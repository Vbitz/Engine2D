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
VertexBuffer2D.fromFile(string filename);

void VertexBuffer2D.prototype.addVert(Vector2f vertPositon, Color4f col = Color4f.White, Vector2f uv = null);
void VertexBuffer2D.prototype.setAutoUV(bool val);

void VertexBuffer2D.prototype.rect(Vector2f p1, Vector p2);
// Potentioly other Draw2D Methods

VertexBuffer2D.save(string filename);

number VertexBuffer2D.prototype.verts;
```
