# AxisNeuronFakeServer
If you develop a game with motion capture with AxisNeuron and you're tired of wearing a mocap suit for testing - this project for you.
With this project you can record animation in file (in BVH format), run this FakeServer and server will looply broadcast your animation by TCP in real-time. So you don't need launched AxisNeuron, weared suit and etc.

# Compilation
1) create project and add main.c, bvhreader.h and bvhreader.c files into it.
2) CSFML binaries are in repository, so you don't need to download it
3) link CSFML binaries to your project
4) build and launch

# Dependences
- libc
- CSFML (network and system module)
