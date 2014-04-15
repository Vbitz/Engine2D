## process.graphicsVersion
Bascily a user agent string, something like `Engine/GIT_beb5249 (Mac OS X 10_9_1) OpenGL/4.0`

## sys.hasExtention && sys.getExtentions
At least in javascript this is something scripters are never going to touch. They can detect any stuff they need with `process.supports`.
I may consider this as a extention API but not inside of process

## sys.getMaxTextureSize
This will find a new home inside of some Texture object as a static propety

## process.saveScreenshot
This might be intergrated with some sort of screen texture fetch something like `process.getWindowFramebuffer().save(filename)`

## $ && process.emit
With the adition of the event API this is not really needed anymore. There's still a place for this API as I extend the event system but for day to day tasks the event API should work fine