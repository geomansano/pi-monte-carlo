# Monte Carlo π Estimation in C

This project implements a **Monte Carlo algorithm** to estimate the value of π (pi) using **multithreading** with POSIX threads (`pthread`). It demonstrates numerical approximation, random number generation, and parallel computation in C.

---

## Overview

The algorithm estimates π by generating random points `(x, y)` in a square that encloses a unit circle. Points that satisfy the condition:

```
x² + y² ≤ 1
```

are counted as inside the circle. The ratio of points inside the circle to the total number of points approximates π/4. Therefore:

```
π ≈ 4 * (points_inside_circle / total_points)
```

By using multiple threads, the computation is accelerated, and each thread uses a unique random seed to ensure independent random sequences.

---

## Features

* Parallel computation with **POSIX threads**.
* Configurable **number of threads** and **points per thread**.
* Unique random seeds per thread for statistical independence.
* High precision approximation of π using a large number of points.

---

## Usage

### Compile

```bash
gcc -pthread monte_carlo_pi.c -o monte_carlo_pi -lm
```

### Run

```bash
./monte_carlo_pi [TOTAL_POINTS] [NUM_THREADS]
```

* `TOTAL_POINTS` (optional) — points per thread (default: 2,097,152)
* `NUM_THREADS` (optional) — number of threads (default: 32)

**Example:**

```bash
./monte_carlo_pi 1000000 16
```

The program outputs:

```
hits: <points inside circle>
Pi: <estimated value>
```
