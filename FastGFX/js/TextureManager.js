var parseBmFont = require('parse-bmfont-ascii')

import { SpritesTexture } from './SpritesTexture.js'

var TextureManager = function(gl,loader) {
  this.gl = gl
  
  this.loader = loader

  this.loadingSprites = new Map()
  this.loadedSprites = new Map()

  this.spriteTextures = []
  this.maxSize = 512

  this.spacing = 2
}
var absUrlPattern = /^https?:\/\//i
TextureManager.prototype.loadSprite = function(sprite) {
  var name = Module.Pointer_stringify(Module._fgfx_Sprite_getName(sprite))
  console.debug("SPRITE REQUESTED", name, sprite)
  var loaded = this.loadedSprites.get(name)
  if(loaded) {
    console.debug("SPRITE LOAD AND PUSH", name)
    Module._fgfx_Sprite_setTextureFragment(loaded.sprite, loaded.texture,
      loaded.coords.xmin, loaded.coords.ymin, loaded.coords.xmax, loaded.coords.ymax,
      loaded.coords.width, loaded.coords.height, loaded.preloaded || false)
    return;
  }
  var loading = this.loadingSprites.get(name)
  if(loading) {
    loading.sprite = sprite
    return;
  }
  var sprite = {
    coords: null,
    sprite: sprite,
    texture: 0,
    preloaded: false,
    name: name,
    image: null
  }
  this.loadingSprites.set(name, sprite)
  this.loader.loadSpriteImage("assets/"+name).then((image) => {
    //console.log("SPRITE LOADED",image)
    sprite.image = image
    var bigger = false
    var added = false
    while(image.width+this.spacing*2>this.maxSize || image.height+this.spacing*2>this.maxSize) {
      this.maxSize*=2
      bigger = true
    }
    if(!bigger) {
      for (var i = 0; i < this.spriteTextures.length; i++) {
        if (this.spriteTextures[i].tryAddSprite(sprite)) {
          added = true;
          break;
        }
      }
    }
    if(!added) { // new texture
      var ntex = new SpritesTexture(this.gl,this.maxSize,this.spacing)
      this.spriteTextures.push(ntex)
      ntex.tryAddSprite(sprite)
    }
  })
}
TextureManager.prototype.loadSpriteFont = function(font) {
  var name = Module.Pointer_stringify(Module._fgfx_SpriteFont_getName(font))
  console.info("LOADING FONT",name)
  var imagePromise = this.loader.loadFontImage("assets/" + name)
  var dataPromise = this.loader.loadFontData("assets/" + name)
  imagePromise.then((image)=> dataPromise.then((data)=> this.uploadPackedSpriteFont(font,data,image)))
}
TextureManager.prototype.loadTexture = function(texture) {
  var name = Module.Pointer_stringify(Module._fgfx_Texture_getName(texture))
  var imagePromise = this.loader.loadTextureImage("assets/" + name)
  //console.error("LOAD TEX IMAGE", "assets/" + name)

  imagePromise.then((image) => {
    var textureId = Module._fgfx_Texture_getTexture(texture);
    console.error("LOADED TEX IMAGE", "assets/" + name, textureId)
    var tex
    if(textureId == -1) {
      tex = gl.createTexture()
      while(GL.textures.length==0) GL.textures.push(null)
      textureId = GL.textures.length
      GL.textures[textureId] = tex
    } else {
      tex = GL.textures[textureId]
    }
    var gl = this.gl
    gl.bindTexture(gl.TEXTURE_2D, tex)
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
    gl.generateMipmap(gl.TEXTURE_2D);
    Module._fgfx_Texture_setTexture(texture, textureId)
  })
}
TextureManager.prototype.reload = function() {
  var cnt = Module._fgfx_getSpritesToLoadCount()
  for(var i=0; i<cnt; i++) {
    var sprite = Module._fgfx_getSpriteToLoad(i)
    this.loadSprite(sprite)
  }
  Module._fgfx_clearSpritesToLoad()

  var cnt = Module._fgfx_getSpriteFontsToLoadCount()
  for(var i=0; i<cnt; i++) {
    var font = Module._fgfx_getSpriteFontToLoad(i)
    this.loadSpriteFont(font)
  }
  Module._fgfx_clearSpriteFontsToLoad()

  for (var i = 0; i < this.spriteTextures.length; i++) {
    this.spriteTextures[i].upload()
  }

  var cnt = Module._fgfx_getTexturesToLoadCount()
  for(var i=0; i<cnt; i++) {
    var texture = Module._fgfx_getTextureToLoad(i)
    this.loadTexture(texture)
  }
  Module._fgfx_clearTexturesToLoad()
}
TextureManager.prototype.clean = function(engineTime) {
  for (var i = 0; i < this.spriteTextures.length; i++) {
    this.spriteTextures[i].clean(engineTime)
  }
}
TextureManager.prototype.uploadPackedSpriteFont = function(spriteFont, fntData, image) {
  var data = parseBmFont(fntData)
  console.debug("PRELOAD PACKED FONT SPRITES", fntData, image)
  for(var i = 0, l=data.chars.length; i<l; i++) {
    var char = data.chars[i]
    var c = String.fromCharCode(char.id)
    //console.log("LOAD CHAR",c)
    var spriteName = "font_"+name+"_"+c
    this.loadingSprites.set(spriteName,{
      char: char,
      coords: null,
      sprite: null,
      texture: 0,
      preloaded: true,
      name: spriteName
    })
  }

  /// LOAD TEXTURE TO GPU MEMORY
  var w = image.width
  var h = image.height

  var gl = this.gl
  var texture = gl.createTexture()
  gl.bindTexture(gl.TEXTURE_2D, texture)
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE,image)
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
  gl.generateMipmap(gl.TEXTURE_2D);

  while(GL.textures.length == 0) GL.textures.push(null)
  var textureId = GL.textures.length
  GL.textures.push(texture)

  console.debug("LOADED SPRITES FONT TEXTURE",textureId)
  /// LOAD FONT SPRITES
  //console.log("FONT DATA",data)
  for(var i = 0, l = data.chars.length; i<l; i++) {
    var char = data.chars[i]
    var c = String.fromCharCode(char.id)
    var spriteName = "font_"+name+"_"+c
    console.debug("FONT SPRITE LOADED",spriteName)
    var loaded = this.loadingSprites.get(spriteName)
    loaded.coords = {
      xmin: char.x/w,
      ymin: char.y/h,
      xmax: (char.x+char.width)/w,
      ymax: (char.y+char.height)/h,
      width: char.width,
      height: char.height
    }
    loaded.texture = textureId
    this.loadingSprites.delete(spriteName)
    this.loadedSprites.set(spriteName,loaded)
    console.log("SPR",spriteName,"ENG",this.engine)
    var cppSpriteName = Module.allocate(Module.intArrayFromString(spriteName), 'i8', ALLOC_NORMAL)
    if(!loaded.sprite) loaded.sprite = Module._fgfx_getSprite(cppSpriteName)
    Module._free(cppSpriteName)
    Module._fgfx_Sprite_setTextureFragment(loaded.sprite,loaded.texture,
      loaded.coords.xmin, loaded.coords.ymin, loaded.coords.xmax, loaded.coords.ymax,
      loaded.coords.width, loaded.coords.height, true)
    console.debug("SET CHAR",char,"SP",loaded.sprite,"AT",spriteFont)
    Module._fgfx_SpriteFont_setCharacter(spriteFont, char.id, loaded.sprite, char.width, char.height,
      char.xoffset, char.yoffset, char.xadvance)
  }
  Module._fgfx_SpriteFont_setHeight(spriteFont,data.common.lineHeight,data.common.base)
}
TextureManager.prototype.loadPackedSprites = function(sprites,imageUrl) {
  var preloadImageTask = preload.preloadImageTask(imageUrl)
  console.debug("PRELOAD PACKED SPRITES",sprites)
  for(var k in sprites) {
    var coords = sprites[k]
    this.loadingSprites.set(k,{
      name: k,
      coords : coords,
      sprite: null,
      texture: 0,
      preloaded: true
    })
  }
  /// TODO: add to images preloader
  preloadImageTask.start()
  preloadImageTask.resultPromise.then((function(image) {
    /// LOAD TEXTURE TO GPU MEMORY

    var gl=this.gl
    var texture=gl.createTexture()
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE,image)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
    gl.generateMipmap(gl.TEXTURE_2D);

    while(GL.textures.length==0) GL.textures.push(null)
    var textureId=GL.textures.length

    GL.textures.push(texture)
    console.debug("LOADED SPRITES TEXTURE",textureId)
    /// LOAD SPRITES
    for(var k in sprites) {
      console.debug("SPRITE LOADED",k)
      // var coords = sprites[k]
      var loaded = this.loadingSprites.get(k)
      loaded.texture = textureId
      this.loadingSprites.delete(k)
      this.loadedSprites.set(k,loaded)
      if(loaded.sprite) {
        console.debug("SPRITE PUSH",k)
        Module._api_fgfx_Sprite_setTextureFragment(loaded.sprite,loaded.texture,
          loaded.coords.xmin, loaded.coords.ymin, loaded.coords.xmax, loaded.coords.ymax,
          loaded.coords.width, loaded.coords.height)
        loaded.sprite.preloaded = loaded.preloaded || false
      }
    }
  }).bind(this))
}

export { TextureManager }
export default TextureManager
