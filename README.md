<div id="top"></div>



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/lucmahoux/CubicUniverse">
        <!--<img src="images/logo.png" alt="Logo" width="80" height="80"> -->
  </a>

<h3 align="center">CubicUniverse</h3>

  <p align="center">
    A voxel game based on Minecraft, coded in C with OpenGL.
    <br />
    <a href="https://github.com/lucmahoux/CubicUniverse"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://lucmahoux.github.io">Our Project Web Site</a>
    ·
    <a href="https://github.com/lucmahoux/CubicUniverse/issues">Report Bug</a>
    ·
    <a href="https://github.com/lucmahoux/CubicUniverse/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project


We rebuilt some of the iconic game Minecraft.

<p align="right">(<a href="#top">back to top</a>)</p>



### Built With

* [C](https://devdocs.io/c/)
* [OpenGL](https://www.opengl.org//)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

List prerequisites here

* In order to be able to compile the project, you must install the following dependencies:
  ```sh
  sudo apt-get install libglfw3
  sudo apt-get install libglfw3-dev
  sudo apt-get install libglew-dev
  sudo apt-get install freeglut3
  sudo apt-get install freeglut3-dev
  ```

* Moreover, regarding the unit-tests part of the project, you must install Criterion.
  You can get it from the github repository [Criterion](https://github.com/Snaipe/Criterion).
  This project uses Meson buildsystem to generate the executable along with Ninja,
  therefore you must also install the following dependencies:
  ```sh
  sudo apt-get install meson
  sudo apt-get install ninja
  ```

### Installation

1. Go on [https://lucmahoux.github.io](https://lucmahoux.github.io)
2. Clone the repo
   ```sh
   git clone https://github.com/lucmahoux/CubicUniverse.git
   ```
3. To build the project, you simply have to go to the root of the project and run :
   ```sh
   ./compile
   ```
   if you want to build & run the project.
   ```sh
   ./compile debug
   ```
   if you want to build & run the test-suite.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

HOW TO PLAY CUBICUNIVERSE - Run it & gaze at the beauty of a skybox & some blocks :)


<p align="right">(<a href="#top">back to top</a>)</p>
