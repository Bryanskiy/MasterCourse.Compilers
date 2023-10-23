#pragma once

#include "IR.hh"
#include "function.hh"

//                 +-----+
//                 |  0  |
//                 +-----+
//                    |
//                    V
//                 +-----+    +-----+
//          +----->|  1  |--->|  2  |
//          |      +-----+    +-----+
//          |         |
//          |         V
//          |      +-----+
//          -------|  3  |
//                 +-----+
jade::Function example1();

//                 +-----+
//                 |  0  |
//                 +-----+
//                    |
//                    V
//                 +-----+
//       ----------|  1  |---------
//       |         +-----+        |
//       |                        |
//       V                        V
//     +-----+     +-----+     +-----+
//     |  2  |  ---|  3  |<----|  4  |
//     +-----+  |  +-----+     +-----+
//        |     |                 |
//        |     V                 |
//        |  +-----+     +-----+  |
//        +->|  5  |<----|  6  |<-+
//           +-----+     +-----+
//
jade::Function example2();
