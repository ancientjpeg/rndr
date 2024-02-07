`rndr` is a lightweight rendering engine and wrapper around the in-progress [WebGPU spec](https://www.w3.org/TR/webgpu/). It is intended as a middle-ground between platforms like [Unity](https://unity.com) and [TouchDesigner](https://derivative.ca) and direct usage of low-level graphics APIs. It deigns to give users, in particular technically-minded artists, complete control over their hardware without the hassle of writing hundreds of lines of boilerplate or learning an entire framework.

## Features

- Slim, templated matrix implementation
- Basic `.obj` parser
  - Currently only capable of reading vertex and face information

## Intended Features

- Graphics-related matrix helpers such as rotation and scale
- Easy API for
- Fully-featured `.obj` parser
  - `.mtl` parser (?)
