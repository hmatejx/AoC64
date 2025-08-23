# Solving the Advent of Code 2021 in C++ on the Commodore 64 

## TL;DR

- I solved the 2021 series of the Advent of Code (AoC) puzzles in C/C++ on a Commodore 64 (C64), closing the circle, and finally reaching **500 stars**.
- I am not the first one to achieve this on a C64. According to the best of my knowledge, Jukka Jylänki [did it first for AoC 2023](https://clb.confined.space/aoc2023/), and his magnificent and pioneering achievement was my key inspiration.
- To make the solving experience feasible and efficient, I gradually built up a helper library of the most commonly needed data structures. The library grew hand-in-hand with the puzzles. Whenever I hit a need (stack, heap, hash set, ...), I stopped, implemented it, and then reused it later whenever possible. If I noticed a bug or sub-optimal code, I fixed it.
- Some building blocks, most importantly **`reucpy.h`**, were kindly shared with me by Jukka Jylänki. Without that REU DMA helper, many solutions would just have been impractical.
- The C64 is a very limited platform. While some day's solutions take only a few seconds, or even less than a second, there are many days, especially towards the end, where the run time is measured in minutes or even hours!
- I hope my journey will inspire others to try. All I am asking for, in case you use or repurpose some of my work, is a slight nod in my direction.
- This document is neither a tutorial nor a systematic breakdown of the solutions but more like a high-level summary with some key highlights.

![](img/AoC64-2021.png)

---

## Introduction

The Advent of Code series of programming puzzles has definitely re-ignited my interest for puzzles and my passion for coding. I first discovered AoC in 2017. I solved that year in [R](www.r-project.org), a statistics-oriented programming language I have been using frequently at work.

n the subsequent years I continued using R, mainly because of my familiarity and fluency with it. Next year I have decided to switch to Python. Python is, after all, very well suited for such tasks. Due to lack of time I skipped a few year's event, did one year in R again, and one in Python. 

Experiencing how effective Python is at parsing the inputs (some days it almost felt like cheating), expressing the algorithms, and coding the logic, I went on binge-solving the remaining years. In doing so I mostly used Python. But then it occurred to me that I was missing an opportunity to learn new languages. Therefore I switched to Go and Rust to solve some other years. I also planned to try out C++ and JavaScript, but since I was not careful with my pacing, I quickly ran out of years to be solved.

With only one year (2021) still remaining and two languages to choose from, I made the choice and decided to solve it in C++, a language I already "knew" but hadn't used for a long time. Okay, but how did I end up on a C64, you ask? The answer is, simply, that the _stars aligned_:

- I received a fully functional (and nicely preserved!) C64 as a gift from my former coworkers. They knew I'm into vintage computers, so it was a really nice and fitting gift.  
- I wanted to "up the ante", to use [Reddit's AoC](https://www.reddit.com/r/adventofcode/) parlance, by making the puzzles more difficult. The exercise of solving all the past years had definitely increased my skills and repertoire of key algorithms (recursion, DFS, BFS, Dijkstra, A*, memoization, modular arithmetic, ...), so the puzzles became easier and easier to solve.  
- I stumbled upon the awesome pioneering achievement of Jukka Jylänki and was deeply impressed; not only did he solve all days, he also live-streamed the achievement!  
- And lastly, the availability and maturity of the [llvm-mos](https://llvm-mos.org) compiler and the [VICE C64](http://vice-emu.sourceforge.net/) emulator made the whole thing feasible also from a workflow point of view. My MOS6502 assembly skills are far too limited to attempt solving anything but the simplest of the puzzles (and let's not get into the time commitment that would entail...).

Another thing to note is also the element of nostalgia. My first computer ever was a Commodore 128. I fondly remember using the tape drive to load and save my BASIC programs. I never had a floppy drive. And when the tape drive failed, I simply used pen and paper to "store" and "reload" my work. I got quite adept at re-typing the code from scratch from my notebook when starting a new coding session.  

So while the C64 is not exactly the same as the C128, the similarity of the experience still evoked some nice childhood memories and at the same time a reflection on how much things have progressed since then. To be able to close the AoC journey on the C64, solving contemporary puzzles using a modern language on hardware that is very similar to my first computer ever, was an opportunity impossible to pass.  

The last thing to note in the introduction: please don't expect too much from my code. It's been almost 20 years since I have last used C++. The language has evolved substantially since then and it continues to evolve. For example, I am reading discussions about the latest C++23/25/26 standards and I am struggling to understand even half of the nuance.  

---

## Platform Limitations

The Commodore 64 / C++ platform has multiple significant limitations that defined the boundary conditions of this experiment:

### Hardware
- **RAM**: only 64 KB addressable, minus screen/KERNAL and other reserved areas, effectively landing at slightly below 50 KB, and this include the program executable itself, the input, and any space needed to perform the computation.
- **CPU**: MOS 6510, ~1 MHz. Anything algorithmically heavy is measured in minutes/hours.
- **Registers**: 8-bit A, X, Y; 16-bit addressing. Painful for the compiler, but luckily the compiler can use the *zero page* (the first 256 Bytes of the C64 memory map) as additional imaginary registers for the compiler due to faster access and additional addressing modes.

### Software/Compiler
- **No STL**: too big, too dynamic, too memory-hungry. 
- **No exceptions / RTTI**.  
- **Optional REU**: with Jukka Ylänki's **`reucpy.h`** helper, the REU becomes a practical extended memory and DMA scratchpad.

### C++ Subset
I deliberately stayed in the ["orthodox" subset](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b): fixed-capacity templates with a MAX_SIZE parameter, no recursion, explicit arrays and iterators.  

---

## Helper Library Written Along the Way

In contrast to the basic I/O & scren helpers, the library was not something I designed in advance. Instead, it **emerged naturally as I solved the puzzles**.  

- **Day 01–08**: Very simple problems, solved with plain arrays and loops.
- **Day 09**: Needed a `queue.h` for BFS/flood-fill.
- **Day 10**: Needed a `stack.h` for bracket matching.
- **Day 12**: graph traversal forced me to write a `hashset.h`.  
- **Day 15**: Implementing Dijkstra required a priority queue, so I implemented `min_heap.h`
- **Day 18**: Built a compact binary tree, though this one I never generalized (TODO). 
- **Days 19, 21, 22, 23:** These exploded beyond 64 KB RAM. I wrote REU-backed variants:  
    - `stack_reu.h`  
    - `hashmap_reu.h`  
    - `hashset_reu.h`  
    - `min_heap_reu.h`

I wrote most of these structure from scratch, but for some (e.g. min_heap) I searched for example C/C++ code on the internet and modified it to the platform.

Other relevant parts of the library:  
- **Screen & I/O helpers** (`AoC64.h`) to show banners, progress indicator, and measure elapsed time.  
- **Hashing** (`pearson.h`) for compact key indexing.  
- **REU operations**: provided by Jukka's `reucpy.h`. This file was _the_ enabler for large datasets. I allowed using the REU as a scratch storage for arbitrary data as well as a further performance hack way to quickly copy or set data in main memory via REU DMA functions.

---

## Workflow & Toolchain

- **Compiler**: [llvm-mos](https://llvm-mos.org).  
- **Emulator**: [VICE](http://vice-emu.sourceforge.io/).  
- **Dual builds**: All the code can also be compiled and run on the host (Win 10 with VSCODE and Microsoft Visual C++) for testing. This was very helpful for debuging my solutions (no need to wait minutes or hours).
- **Workflow**:
    - Encode the puzzle input as a header file `test.h` or `input.h` (an important decision I made was that I will spend too much time on I/O procedures for reading and parsing files on the C64; I still did parsing of strings in the code, but if it was trivial to transform the string to an array, I did it in the input). 
    - Write/debug core algorithm on host.
    - Cross-compile with llvm-mos.
    - Test binary in VICE and finally on real C64 hardware.

---

## Day-by-Day Highlights

Not every puzzle deserves an expanded story with respect to the C64 setting. I have clearly marked (with ⚠) and underlined the ones that were especially challenging given the C64 constrains . Here is the summary:

- **Day 01 (Sonar Sweep)**: pure array iteration, no need for extra structures, part 2 was done with a sliding window.
- **Day 02 (Dive!)**: array-based solution with no extra structures, just careful pointer use and `atoi` for parsing.
- **Day 03 (Binary Diagnostic)**: plain arrays with bit masking and shifting, both parts done in single pass.
- **Day 04 (Giant Squid)**: entirely on fixed-size arrays, no custom containers.
- **Day 05 (Hydrothermal Venture)**: Splits the grid into tiles, rasterizing only the lines that intersect each tile to keep memory at ~8 KB.
- **Day 06 (Lanternfish)**: with a fixed-size bucket counter array, fits in a few bytes and runs in constant time.
- **Day 07 The Treachery of Whales)**: simple array math, part 1 is O(N^2) but fast enough on input size leveraging the median, part 2 leverages the mean to avoid scanning all positions.
- **Day 08 (Seven Segment Search)**: a very direct string-manipulation solution, relying only on string length and intersection counts to reconstruct the mapping
- **Day 09 (Smoke Basin)**: classic flood fill rephrased for C64 constraints - no recursion, small fixed queue, manual top-3 tracking without sorting. This is the first day that required me to write [a simple array-backed Queue]() for storing the open set, but the closed set was a simple 2D byte map (bits would take 1/8th of the space, but speed was more important).
- **Day 10 (Syntax Scoring)**: an archetypal stack problem, so I wrote [a simple array-backed Stack]() to hold the currently open brackets. For part 2 I used a negative return from `corrupted()` to flag incomplete lines.
- **Day 11 (Dumbo Octopus)**: iterative flood-fill style propagation with a stack, carefully avoiding recursion and duplicate pushes. REU is used only for quick clearing of state arrays each step.
- **Day 12 (Passage Pathing)**: compact, iterative DFS over a small graph, with state objects capturing the cave visitation rules. The C64's constraints drove the fixed-capacity stack and array sizes.
- **Day 13 (Transparent Origami)**: efficient set operations via [a simple custom HashSet]() to make folding feasible on the C64, and the final part demonstrates the nostalgic thrill of printing out a "message" on screen with no need to fake that retro feeling on a C64.
- **Day 14 (Extended Polymerization)**: instead of storing the whole polymer this solution implements a pair-counting approach, with REU-accelerated memory copies to make it even faster.
- **⚠ <u>Day 15 (Chiton)</u>**: this one was a challenge due to memory requirements. Not only I needed to implement [a min Heap]() (a priority queue), I also needed to find a way to store the massive closed set on the REU to make a full Dijkstra feasible on the C64 for very large grids.
- **Day 16 (Packet Decoder)**: no special structures but low level bit manipulation, implements a full bit-level recursive packet parser and evaluator directly on the C64.
- **Day 17 (Trick Shot)**: brute-force simulation of possible velocities with early pruning for `vx`, efficient enough on the C64 given small search space.
- **Day 18 (Snailfish)**: an array-based compact binary tree with manual traversal and no recursion. Here I needed [help from the internet](https://github.com/FransFaase/AdventOfCode2021/blob/main/src/day18_4sol.cpp). I had the right idea in my mind but had trouble with the implementation. For some reason I did not make this structure generic yet, I probably simply forgot and the need for a tree did not reappear in later days.
- **⚠ <u>Day 19 (Beacon Scanner)</u>**: one of the heaviest problems of AoC 2021, an absolute monster for the C64. The solution leans hard on custom data structures plus the REU for memory management. First, I needed to implement a 24 bit [Pearson hash]() to sort calculate distance fingerprints and sort them with the Heap and then store all the sorted distance fingerprints on the REU using [a REU version of the Stack](). The beacon matches were  detected with a nice linear scan through sourted distance fingerprints. Transformations were found by trying all 24 rotation matrices. The first scanner was taken as origin, and then iteratively overlapping scanner were found with transformations applied to bring them into the same reference frame. Finally, all the beacons were put into a HashSet so that they could be counted.
- **Day 20 (Trench Map)**: implements infinite-grid image enhancement via a bit-packed double-buffered image array, with REU-accelerated clears, making it feasible to simulate 50 iterations on the C64.
- **⚠ <u>Day 21 (Dirac Dice)</u>**: the "deterministic" part is simple, but for part 2 I needed to implement [a REU based HashMap]() for memoization used to make the the vast search space of the recursive algorithm tractable on a C64.
- **⚠ <u>Day 22 (Reactor Reboot)</u>**: implements the signed volumes solution (inclusion–exclusion principle), storing the large evolving set in the REU to make full Part 2 feasible on the C64.
- **⚠ <u>Day 23 (Amphipod)</u>**: this was was very challenging from a coding perspective, I kept on changing the way I was represent the state because I didn't like how rule checking code looked. Because I was kind of stuck deciding between representations, I checked the internet for solutions and took various inspirations and parts form them. Technically, this is an implementation of Dijkstra on puzzle states, [a REU-backed HashMap]() and a [REU-backed min Heap]() to handle the massive state space, making even the monstrous part 2 solvable on a C64.
- **Day 24 (Arithmetic Logic Unit)**: it took me way too much time to figure out what the code does, but once I did, I immediately saw that a simple stack will do the job.
- **Day 25 (Sea Cucumber)**: grid update in two phases with a stack used as a staging buffer.

The C64 run time and memory requirements for my solutions to each day of Advent of Code 2021 are shown in the table below.

| Day  | Time (s) | REU                     |
| ---- | -------- | ----------------------- |
| 1    | 0.86     |                         |
| 2    | 1.54     |                         |
| 3    | 3.78     |                         |
| 4    | 16.90    |                         |
| 5    | 312.14   | †                       |
| 6    | 0.96     | †                       |
| 7    | 208.60   |                         |
| 8    | 5.68     |                         |
| 9    | 12.12    |                         |
| 10   | 1.40     |                         |
| 11   | 21.72    | †                       |
| 12   | 2369.80  |                         |
| 13   | 11.96    |                         |
| 14   | 10.70    | †                       |
| 15   | 9332.94  | ‡   (≥ 256 kB required) |
| 16   | 2.48     |                         |
| 17   | 46.46    |                         |
| 18   | 503.24   |                         |
| 19   | 449.76   | ‡   (≥ 256 kB required) |
| 20   | 2578.20  | †                       |
| 21   | 1594.54  | ‡   (≥ 512 kB required) |
| 22   | 2686.50  | ‡   (≥ 256 kB required) |
| 23   | 10879.40 | ‡   (≥ 8 MB required)   |
| 24   | 1.80     |                         |
| 25   | 1249.14  |                         |

† - REU used only for faster memory ops (DMA).  
‡ - REU essential, minimum size listed.

![](img/timing.png)

---

## Performance Notes

- **Fixed-capacity containers**: always know your maximum upfront.
- **Memory reuse**: if you don't need some data anymore (e.g. input already parsed), use its memory location for storing other data.
- **No recursion**: explicit stacks/queues for DFS/BFS.
- **24-bit integers**: `_BitInt(24)` was a sweet spot forn some cases because a 32 bit int would waste too much memory.
- **REU as lifeline**: with `reucpy.h`, bulk copy and zeroing operations are possible; without it, several later puzzles simply wouldn't fit into memory.

---

## Lessons Learned

- Writing the **library on demand** kept me motivated. Each new structure was born from necessity, not speculation.
- **Constraints improve clarity**: you can't fake efficiency on a 1 MHz CPU.
- **Collaboration matters**: Jukka Jylänki's shared code was crucial to get me started and has probably saved me weeks.
- And finally: **AoC is still fun after years**, even more so when combined with nostalgia and new learning.

---

## Acknowledgements

- **Jukka Jylänki** - inspiration, pioneer, and provider of `reucpy.h`.  
- **llvm-mos team** - for the awesome compiler.  
- **VICE team** - for reliable (and fast - warp mode) emulation.  
- **Advent of Code team and especially [u/topaz2078](https://www.reddit.com/user/topaz2078/)** - for puzzles that spark this kind of crazy project.  

---

## License

The helper library is free to use and modify. If you do, a little nod in my direction would be appreciated.  