#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "boids.h"
#include "util.h"

#include "boids-shaders.h"

#define KEY_ESCAPE 0x1b
#define KEY_ENTER 0xd
#define FRAME_RATE 60
#define MAX_SPEED 7

#define NUM_BOIDS 100

// the simulation is actually nondeterministic, but this will ensure the start
// positions of the boids are predictable
#define PREDICTABLE true

using namespace std;

bitset<0x100> keysPressed;

enum Impl {
  BRUTE,
  INDEX,
  SPREAD_INDEX
};

static struct State {
  Params params;
  Buffers buffers;
  GLuint vao[2];
  GLuint queries[7];
  GLuint activeBuffer;

  int winWidth, winHeight;

  GLuint *offsets;
  range *ranges;

  GLuint boidDispatch, cellDispatch, minorCellDispatch;
  GLuint nextQueryNum;

  // settings
  bool paused, lockFramerate, printTiming,
    changingNumBoids, changingImpl;
  GLuint numBoidsIn;

  bool timerActive;

  uint32_t lastTime;
  uint64_t frameOffset;

  enum Impl impl;

  random_device random_dev;
  default_random_engine random_eng;
  uniform_real_distribution<GLfloat> uniform_angle,
    uniform_x, uniform_y;
  int lastDistW, lastDistH;

  State(const bool predictable) : offsets(NULL), ranges(NULL),
      nextQueryNum(0),
      timerActive(false), frameOffset(0), impl(SPREAD_INDEX),
      uniform_angle(0, GLfloat(2 * M_PI)), lastDistW(0), lastDistH(0) {
    if (predictable) {
      random_eng = default_random_engine();
    } else {
      random_eng = default_random_engine(random_dev());
    }
  }

  vec2 randomPoint() {
    if (lastDistW != winWidth || lastDistH != winHeight) {
      lastDistW = winWidth;
      lastDistH = winHeight;
      uniform_x = uniform_real_distribution<GLfloat>(0, GLfloat(winWidth));
      uniform_y = uniform_real_distribution<GLfloat>(0, GLfloat(winHeight));
    }

    return vec2(uniform_x(random_eng), uniform_y(random_eng));
  }
} state(PREDICTABLE);

static boid randomBoid() {
  GLfloat angle = state.uniform_angle(state.random_eng);

  boid b;
  b.location = state.randomPoint();
  b.velocity = vec2(cos(angle) * MAX_SPEED, sin(angle) * MAX_SPEED);
  return b;
}

static void queryTime() {
  if (state.printTiming && state.nextQueryNum < arraySize(state.queries)) {
    glQueryCounter(state.queries[state.nextQueryNum++], GL_TIMESTAMP);
  }
}

static void writeNsAsMs(ostream &out, GLuint64 ns) {
  GLuint64 ms = ns / 1000000;
  ns %= 1000000;
  out << ms << "." << setfill('0') << setw(6) << ns;
}

static void printTiming() {
  if (state.printTiming) {
    // busy-wait for the queries to be available - this loop normally terminates
    // immediately
    GLint done;
    do {
      glGetQueryObjectiv(state.queries[state.nextQueryNum - 1], GL_QUERY_RESULT_AVAILABLE, &done);
    } while (!done);

    GLuint64 first, last;
    glGetQueryObjectui64v(state.queries[0], GL_QUERY_RESULT, &first);
    last = first;
    for (uintptr_t i = 1; i < state.nextQueryNum; ++i) {
      GLuint64 now;
      glGetQueryObjectui64v(state.queries[i], GL_QUERY_RESULT, &now);
      writeNsAsMs(cout, now - last);
      cout << "ms ";
      last = now;
    }
    cout << "(total: ";
    writeNsAsMs(cout, last - first);
    cout << "ms)" << endl;

    state.nextQueryNum = 0;
  }
}

static void updateParams() {
  GLuint cellSize = state.params.cellSize;
  vec2 winScale = vec2(GLfloat(state.winWidth), GLfloat(state.winHeight));
  state.params.winScale = winScale;
  state.params.cellCounts = uvec2(ceil(winScale.x / cellSize), ceil(winScale.y / cellSize));
  state.params.cellCount = state.params.cellCounts.x * state.params.cellCounts.y;
  state.params.projection = transpose(Ortho2D(0, winScale.x, 0, winScale.y));
}

static void initParams() {
  GLfloat separateDist = 25.0f, alignDist = 40.0f, attractDist = 40.0f;

  state.params.edgeDist = 40.0f;
  state.params.edgeFactor = 2.0f;
  state.params.separateDist2 = separateDist * separateDist;
  state.params.separateFactor = 15.0f;
  state.params.alignDist2 = alignDist * alignDist;
  state.params.alignFactor = 0.3f;
  state.params.attractDist2 = attractDist * attractDist;
  state.params.attractFactor = 0.05f;

  state.params.maxSpeed = GLfloat(MAX_SPEED);

  state.params.cellSize = max({separateDist, alignDist, attractDist});
  state.params.mapRatio = 1.0f / state.params.cellSize;

  updateParams();
}

static void allocateCellBuffers() {
  state.cellDispatch = divUp(state.params.cellCount, 128);
  state.minorCellDispatch = divUp(state.params.cellCount, 64);

  // resize the cell buffers
  state.buffers.counts.write(NULL, sizeof(GLuint) * state.params.cellCount);
  state.buffers.offsets.write(NULL, sizeof(GLuint) * state.params.cellCount);
  state.buffers.ranges.write(NULL, sizeof(range) * state.params.cellCount);

  if (state.offsets) delete[] state.offsets;
  if (state.ranges) delete[] state.ranges;

  state.offsets = new GLuint[state.params.cellCount];
  state.ranges = new range[state.params.cellCount];
}

static void allocateBoidBuffers() {
  state.boidDispatch = divUp(state.params.numBoids, 128);

  if (state.impl != BRUTE) {
    GLsizeiptr factor = 1 + 8 * (state.impl == SPREAD_INDEX);
    state.buffers.index.write(NULL, sizeof(GLuint) * state.params.numBoids * factor);
  }
}

static void resizeBoidBuffers() {
  state.buffers.boid[state.activeBuffer ^ 1].write(NULL, sizeof(boid) * state.params.numBoids);
  state.buffers.boid[state.activeBuffer ^ 1].copyFrom(state.buffers.boid[state.activeBuffer]);
  state.buffers.boid[state.activeBuffer].write(NULL, sizeof(boid) * state.params.numBoids);
  state.activeBuffer ^= 1;

  allocateBoidBuffers();
}

static void resize(int width, int height) {
  if (width == 0 || height == 0) return;

  state.winWidth = width;
  state.winHeight = height;
  glViewport(0, 0, width, height);

  GLuint oldCellCount = state.params.cellCount;
  updateParams();

  state.buffers.uniform.write(&state.params, sizeof(state.params));

  if (oldCellCount != state.params.cellCount) {
    allocateCellBuffers();
  }
}

static void display() {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(state.vao[state.activeBuffer]);
  glUseProgram(programs.draw);
  glDrawArrays(GL_POINTS, 0, state.params.numBoids);

  glutSwapBuffers();
}

static void update() {
  ++state.frameOffset;

  state.buffers.boid[state.activeBuffer].bind(0);
  state.buffers.boid[state.activeBuffer ^ 1].bind(1);

  GLuint cellCount = state.params.cellCount;

  queryTime();

  switch (state.impl) {
  case BRUTE: {
    glUseProgram(programs.bruteMove);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    break;
  }
  case INDEX: {
    state.buffers.counts.clear();

    glUseProgram(programs.cellCounts);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime();

    GLuint *counts = (GLuint*) state.ranges;
    state.buffers.counts.read(counts);

    GLuint rollingOffset = 0;
    for (uintptr_t i = 0; i < cellCount; ++i) {
      state.offsets[i] = rollingOffset;
      rollingOffset += counts[i];
    }

    state.buffers.offsets.write(state.offsets, sizeof(GLuint) * cellCount);

    queryTime();

    glUseProgram(programs.index);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // write it again because index mutates the offsets
    state.buffers.offsets.write(state.offsets, sizeof(GLuint) * cellCount);

    queryTime();

    glUseProgram(programs.scanMove);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    break;
  }
  case SPREAD_INDEX: {
    state.buffers.counts.clear();

    glUseProgram(programs.cellCounts);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime();

    state.buffers.ranges.clear();

    glUseProgram(programs.spreadCounts);
    glDispatchCompute(state.minorCellDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime();

    state.buffers.ranges.read(state.ranges);

    GLuint rollingOffset = 0;
    for (uintptr_t i = 0; i < cellCount; ++i) {
      state.offsets[i] = rollingOffset;
      state.ranges[i].offset = rollingOffset;
      rollingOffset += state.ranges[i].count;
    }

    state.buffers.ranges.write(state.ranges, sizeof(range) * cellCount);
    state.buffers.offsets.write(state.offsets, sizeof(GLuint) * cellCount);

    queryTime();

    glUseProgram(programs.index);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime();

    glUseProgram(programs.spreadIndex);
    glDispatchCompute(state.cellDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime();

    glUseProgram(programs.move);
    glDispatchCompute(state.boidDispatch, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    break;
  }
  }

  queryTime();
  printTiming();

  state.activeBuffer ^= 1;

  glutPostRedisplay();
}

static void changeImpl(enum Impl impl) {
  if (impl == state.impl) return;

  state.impl = impl;
  allocateBoidBuffers();
}

static void timerCallback(int value);

static void scheduleTimer() {
  glutTimerFunc(1000 / FRAME_RATE, timerCallback, 0);
}

static void pause() {
  state.paused = true;
  if (!state.lockFramerate) {
    glutIdleFunc(NULL);
  }
}

static void resume(bool initial) {
  bool wasPaused = state.paused;
  state.paused = false;
  if (!state.lockFramerate) {
    glutIdleFunc(update);
  } else {
    if (initial) {
      scheduleTimer();
    } else {
      if (!wasPaused) {
        glutIdleFunc(NULL);
      }
      if (!state.timerActive) {
        timerCallback(0);
      }
    }
    state.timerActive = true;
  }
}

static void timerCallback(int value) {
  if (!state.paused && state.lockFramerate) {
    update();
    scheduleTimer();
  } else {
    state.timerActive = false;
  }
}

static void keyboardCallback(unsigned char key, int x, int y) {
  keysPressed[key] = 1;

  switch (key) {
  case KEY_ESCAPE:
    glutLeaveMainLoop();
    break;
  case 'k':
  {
    uint32_t now = glutGet(GLUT_ELAPSED_TIME), elapsed = now - state.lastTime;
    double fps = state.frameOffset / double(elapsed) * 1000;

    cout << "frame: " << state.frameOffset << " elapsed: " << elapsed << "ms " << fps << " fps" << endl;
    state.lastTime = now;
    state.frameOffset = 0;
    break;
  }
  case 't':
    if (!state.paused) {
      pause();
    }
    update();
    break;
  case 'p':
    if (state.paused) {
      resume(false);
    } else {
      pause();
    }
    break;
  case 'f':
    state.lockFramerate = !state.lockFramerate;
    if (!state.paused) {
      resume(false);
    }
    break;
  case 'c':
    state.printTiming = !state.printTiming;
    break;
  case 's':
    state.changingImpl = !state.changingImpl;
    if (!state.changingImpl) {
      // we pressed 's' twice
      changeImpl(SPREAD_INDEX);
    }
    break;
  case 'i':
    if (state.changingImpl) {
      state.changingImpl = false;
      changeImpl(INDEX);
    }
    break;
  case 'b':
    if (state.changingImpl) {
      state.changingImpl = false;
      changeImpl(BRUTE);
      break;
    }
    state.changingNumBoids = !state.changingNumBoids;
    if (state.changingNumBoids) {
      state.numBoidsIn = 0;
    }
    break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    if (state.changingNumBoids) {
      state.numBoidsIn = state.numBoidsIn * 10 + (key - '0');
      cout << "numBoidsIn: " << state.numBoidsIn << endl;
    }
    break;
  case KEY_ENTER:
    if (state.changingNumBoids) {
      state.changingNumBoids = false;
      GLuint oldNumBoids = state.params.numBoids;
      state.params.numBoids = state.numBoidsIn;
      resizeBoidBuffers();
      if (state.params.numBoids > oldNumBoids) {
        GLuint newBoids = state.params.numBoids - oldNumBoids;
        cout << "newBoids: " << newBoids << endl;

        boid *boids = new boid[newBoids];
        for (GLuint i = 0; i < newBoids; ++i) {
          boids[i] = randomBoid();
        }

        state.buffers.boid[state.activeBuffer].write(boids, sizeof(boid) * oldNumBoids, sizeof(boid) * newBoids);
        delete[] boids;
      }
      state.buffers.uniform.write(&state.params, sizeof(state.params));
    }
    break;
  }
}

static void setupPrograms() {
  programs.draw = glCreateProgram();
  attachShader(programs.draw, GL_VERTEX_SHADER, boidvertex_source);
  attachShader(programs.draw, GL_GEOMETRY_SHADER, boidgeometry_source);
  attachShader(programs.draw, GL_FRAGMENT_SHADER, flatfragment_source);
  linkProgram(programs.draw);

  programs.cellCounts = glCreateProgram();
  attachShader(programs.cellCounts, GL_COMPUTE_SHADER, cellcounts_source);
  linkProgram(programs.cellCounts);

  programs.spreadCounts = glCreateProgram();
  attachShader(programs.spreadCounts, GL_COMPUTE_SHADER, spreadcounts_source);
  linkProgram(programs.spreadCounts);

  programs.index = glCreateProgram();
  attachShader(programs.index, GL_COMPUTE_SHADER, index_source);
  linkProgram(programs.index);

  programs.spreadIndex = glCreateProgram();
  attachShader(programs.spreadIndex, GL_COMPUTE_SHADER, spreadindex_source);
  linkProgram(programs.spreadIndex);

  programs.bruteMove = glCreateProgram();
  attachShader(programs.bruteMove, GL_COMPUTE_SHADER, brutemove_source);
  linkProgram(programs.bruteMove);

  programs.scanMove = glCreateProgram();
  attachShader(programs.scanMove, GL_COMPUTE_SHADER, scanmove_source);
  linkProgram(programs.scanMove);

  programs.move = glCreateProgram();
  attachShader(programs.move, GL_COMPUTE_SHADER, move_source);
  linkProgram(programs.move);
}

static void assocDrawProgram() {
  glUseProgram(programs.draw);

  GLuint vPosition = glGetAttribLocation(programs.draw, "position");

  glGenVertexArrays(2, state.vao);

  // TODO: do we need glUseProgram here?
  glBindVertexArray(state.vao[0]);
  state.buffers.boid[0].bindTo(GL_ARRAY_BUFFER);
  glUseProgram(programs.draw);
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  glBindVertexArray(state.vao[1]);
  state.buffers.boid[1].bindTo(GL_ARRAY_BUFFER);
  glUseProgram(programs.draw);
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

static void setup() {
  initParams();

  setupPrograms();
  assocDrawProgram();

  glGenQueries(arraySize(state.queries), state.queries);

  state.buffers.counts.bind(2);
  state.buffers.offsets.bind(3);
  state.buffers.ranges.bind(4);
  state.buffers.index.bind(5);

  GLuint numBoids = NUM_BOIDS;
  state.params.numBoids = numBoids;
  boid *boids = new boid[numBoids];
  for (GLuint i = 0; i < numBoids; ++i) {
    boids[i] = randomBoid();
  }

  state.buffers.boid[0].write(boids, sizeof(boid) * numBoids);
  state.buffers.boid[1].write(NULL, sizeof(boid) * numBoids);
  delete[] boids;

  allocateCellBuffers();
  allocateBoidBuffers();

  state.buffers.uniform.bind(0);
  state.buffers.uniform.bind();
  state.buffers.uniform.write(&state.params, sizeof(state.params));

  // sets white as color used to clear screen
  glClearColor(1.0, 1.0, 1.0, 1.0);
}

static void displayInfo(const int argc, char * const argv[]) {
  if (argc <= 1) return;

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    cout << "usage: boids" << endl;
    cout << "options:" << endl;
    cout << "--help     display this information" << endl;
    cout << "--version  display version information" << endl << endl;
    cout << "use the following keys to control the simuation during execution:" << endl;
    cout << "ESC  quit the application" << endl;
    cout << "'b'  change the number of boids - press b, type an ordinal number," << endl;
    cout << "     and press enter" << endl;
    cout << "'c'  toggle the printing of compute timings" << endl;
    cout << "'f'  toggle framerate locking between unlimited and " << (1000.0f / FRAME_RATE) << " fps" << endl;
    cout << "'k'  mark the current frame, and print the average fps since the last marked" << endl;
    cout << "     frame" << endl;
    cout << "'p'  pause or play the simulation" << endl;
    cout << "'s'  switch between implementations, by subsequently pressing one of:" << endl;
    cout << "     'b'  brute force implementation" << endl;
    cout << "     'i'  spatial indexing implementation" << endl;
    cout << "     's'  preaggregated spatial indexing implementation" << endl;
    cout << "'t'  pause, and advance the simulation by one frame" << endl << endl;
    cout << "This application depends on OpenGL 4.3.0 or higher." << endl;
    cout << "Report bugs at https://github.com/skeggse/boids" << endl;
    exit(EXIT_SUCCESS);
  } else if (strcmp(argv[1], "--version") == 0) {
    cout << "boids v1.0" << endl;
    exit(EXIT_SUCCESS);
  }
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  displayInfo(argc, argv);

  state.winWidth = 1024;
  state.winHeight = 768;
  state.activeBuffer = 0;
  state.offsets = NULL;
  state.ranges = NULL;

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(state.winWidth, state.winHeight);
  // glutInitWindowPosition(448, 156);
  glutCreateWindow("Boids");

  // use opengl extensions
  glewInit();
  glEnable(GL_PROGRAM_POINT_SIZE);

  setup();

  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboardCallback);

  state.paused = false;
  state.lockFramerate = true;
  state.printTiming = false;

  if (!state.paused) {
    resume(true);
  }

  state.lastTime = glutGet(GLUT_ELAPSED_TIME);

  // enter the drawing loop
  glutMainLoop();

  return 0;
}
