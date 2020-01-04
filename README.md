# What is this?

C code that runs a star system based on parameters you give it in the command line
It's been heavily optimised with parallelisation and vectorisation along with cache optimisation to run as many stars at once as possible
High end machines can achieve up-to 100,000s of stars with reasonable speeds. 

# How do I get it running?

1. Clone the repository
2. Run the make file
3. Simulation will pop-up in a window
4. run `./script` to extract results from the terminal
5. run `python3 sim.py` in the terminal to process these results into an Excel report

## Dependencies

* GCC-8+
* glut
* OpenGL Framework

To fully benefit from optimisations make sure your hardware allows for Intel's AVX capabilities
