# New JavaScript API Concepts

Just trying to get some ideas I've been mulling over for a while out.

## Texture
```javascript
enum TextureType {
    Texture.RGBA,
    Texture.RGB,
    Texture.BGRA,
    Texture.BGR
};

Texture Texture.fromData(FloatBuffer buf, int width, int height, TextureType type);
Texture Texture.fromFile(string filename);

void Texture.prototype.setData(FloatBuffer buf, number width, number height, TextureType type);
void Texture.prototype.setPixel(number x, number y, Color4f col);
Color4f Texture.prototype.getPixel(number x, number y);
void Texture.prototype.setRepeat(bool val);
Texture Texture.prototype.convert(TextureType target);

number Texture.prototype.width;
number Texture.prototype.height;
TextureType Texture.prototype.type;
```

## Effect
```javascript
enum EffectVersion {
    Effect.GLSL_150
}

EffectVersion[] Effect.getSupportedVersions();
Effect Effect.fromJSONFile(string filename);
Effect Effect.fromShaderFilePair(string vertFilename, string fragFilename);
Effect Effect.fromShaderSourcePair(string vertSource, string fragSource);

void Effect.prototype.setVersion(EffectVersion ver);
void Effect.prototype.setDefine(string key, string value);

bool Effect.prototype.compile(); // Throws a error on syntax error or other problems

Effect.prototype.setUniform(string key, [string, Vector2f, Vector3f, Vector4f, Matrix4f] value);
```

## VertexBuffer2D
Basicly the same as the RenderDriver API is right now in C++

```javascript
enum PolygonMode {
    Triangles,
    LineStrip,
    Triangles,
    LineLoop,
    Lines,
    TriangleStrip,
    TriangleFan
};

new VertexBuffer2D(PolygonMode mode); // automaticly expanding

void VertexBuffer2D.prototype.reset();

[x] void VertexBuffer2D.prototype.addVert(Vector2f vertPositon, Color4f col = Color4f.White, Vector2f uv = null);
void VertexBuffer2D.prototype.doAutoUV(); // calculates the bounds of the model and automaticly sets the UVs to fit
void VertexBuffer2D.prototype.setTexture(Texture tex);

void VertexBuffer2D.prototype.rect(Vector2f p1, Vector p2);
// Potentioly other Draw2D Methods

[x] void VertexBuffer2D.prototype.draw();

void VertexBuffer2D.prototype.save(string filename);
void VertexBuffer2D.prototype.load(string filename);

number VertexBuffer2D.prototype.verts;
```
