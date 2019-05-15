Rendering stuff using modern OpenGL

1. Generate an object ID for OpenGL
```
unsigned int objectId = 0;
glGenObject(1, &objectId);
```

2. Bind the object to the target
```
glBindObject(GL_WINDOW_TARGET, objectId);
```

3. Set the options for the [target? object?]
```
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
```

4. Unbind the object from the target
```
glBindObject(GL_WINDOW_TARGET, 0);
```

Set the viewport

`glViewport(GLint x, GLint y, GLsizei width, GLsizei height)`
Example: `glViewport(0, 0, 800, 600)`

Graphics pipeline steps
1. Vertices have vertex attributes
2. The vertex shader takes these attributes and manipulates them
3. The OpenGL implementation (graphics driver) assembles primitives given the vertex data from the vertex shader and hints about it e.g. primitive type.
4. The primitives are sent to the geometry shader if it is available. The geometry shader does stuff like tesselation
5. The GPU calculates all of the data needed in order to display a pixel (a fragment)
6. The fragments are sent to the fragment shader, which calculates the pixel colour and clips stuff out of view
7. The rendered object is blended in with the scene

Normalized device coordinates
It's a bit hard to draw this diagram with just ASCII art, but I tried my best.
```
Z = 0
(-1.0, 1.0)          (0.0, 1.0)              (1.0, 1.0)
 -----------------------|-----------------------
|                       |                       |
|                       |                       |
|    (-0.5, 0.5)     (0.0, 0.5)  (0.5, 0.5)     |
|                       |                       |
|                       |                       |
|(-1.0, 0.0)         (0.0, 0.0)               (1.0, 0.0)
|-----------------------|-----------------------|
|       (-0.5, 0.0)     |     (0.5, 0.0)        |
|                       |                       |
|   (-0.5, -0.5)   (0.0, -0.5)  (0.5, -0.5)     |
|                       |                       |
|                       |                       |
 -----------------------|-----------------------
(-1.0, -1.0)        (0.0, -1.0)             (1.0, -1.0)
```

Vertex Buffer Objects
Use VBOs to manage the vertex data being sent to the GPU
`glGenBuffers(x, vBufID)`
will allocate vertex buffer objects, and return the IDs of the allocated buffers.
`glBindBuffer(GL_ARRAY_BUFFER, vBufID)`
will set the buffer with vBufID to be an "array buffer"
`glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW)`
will upload the data from the "vertices" array to the GPU.
GL_STATIC_DRAW makes the data harder to write to, but faster to render if it doesn't change
GL_DYNAMIC_DRAW or GL_STREAM_DRAW makes the data easier to write to

Shaders
For modern rendering pipelines, which give more control over what is rendered, vertex and fragment shaders are required.
Declare inputs at the top of vertex and fragment shaders using `layout (location = 0) in vec3 aPos;` for example
Shaders first have to be compiled, and then one or more shader(s) are linked into a shader program
```
unsigned int vertexShader;
vertexShader = glCreateShader(shaderType); // Create shader
glShaderSource(vertexShader, 1, &vertexShaderSource, &vertexShaderSize); // Assign source to shader
glCompileShader(vertexShader); // Compile the shader
```
Once the shaders are ready to be used, link them into a program
```
unsigned int shaderProgram;
shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);
```
In the code that renders the scene: `glUseProgram();`
After you're done with the shaders: `glDeleteShader(shaderId);`

Vertex array objects
This is a way to store the configurations for vertex buffer attributes. In order to use it, put glGenVertexArrays(VAO) before the code that sets up the vertex buffers and their respective attributes, and put glBindVertexArray(VAO) when you are done setting up the aforementioned data.
```
unsigned int VAO;
glGenVertexArrays(1, &VAO);
// Bind the Vertex Array Object during setup so that the following OpenGL state changes are recorded in the VAO
glBindVertexArray(VAO);

unsigned int VBO;
glGenBuffers(1, &VBO);
// Render this sh!t
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Give OpenGL information about how the vertex buffer data is presented
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
glEnableVertexAttribArray(0);

// Release the bindings once setup is complete
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);
```
