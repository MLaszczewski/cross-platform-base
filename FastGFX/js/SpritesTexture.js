var STNode=function(x,y,width,height) {
  this.left = null
  this.right = null
  this.image = null
  this.x = x
  this.y = y
  this.width = width
  this.height = height
}
STNode.prototype.insert = function(image,spacing) {
  var box_width=image.width + spacing
  var box_height=image.height + spacing

  if(this.left && this.right) {
    return this.left.insert(image,spacing) || this.right.insert(image,spacing)
  } else {
    // if there's already a sprite here, return
    if(this.image) return null;
    // if we're too small, return
    if((box_width>this.width) || ((box_height)>this.height)) return null
    if((box_width==this.width) && ((box_height)==this.height)) { //if we're just right, accept
      this.image=image
      return this
    } else { // otherwise, gotta split this node and create some kids
      var dw = this.width - (box_width)
      var dh = this.height - (box_height)
      if(dw>dh) {
        this.left = new STNode(this.x, this.y, box_width, this.height)
        this.right = new STNode(this.x+box_width, this.y, this.width-(box_width), this.height)
      } else {
        this.left = new STNode(this.x, this.y, this.width, box_height)
        this.right = new STNode(this.x, this.y+box_height, this.width, this.height-(box_height))
      }
      // insert into first child we created
      return this.left.insert(image,spacing)
    }
  }
}

var SpritesTexture = function(gl,size,spacing) {
  this.gl = gl
  this.size = size

  this.canvas = document.createElement('canvas')
  this.canvas.width = this.size
  this.canvas.height = this.size
  this.ctx = this.canvas.getContext('2d')

  this.sprites = []
  this.spritesByName = {}

  this.spacing = spacing
  this.needUpload = false

  var gl = this.gl
  this.texture = gl.createTexture()
  gl.bindTexture(gl.TEXTURE_2D, this.texture)
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

  while(GL.textures.length == 0) GL.textures.push(null)
  this.textureId = GL.textures.length
  GL.textures.push(this.texture)

}
SpritesTexture.prototype.tryPack = function(sprites) {
  var nodes = new Array(sprites.length)
  var root = new STNode(0,0,this.size,this.size)

  sprites = sprites.sort((function(a,b) {
    return Math.max(b.width,b.height)-Math.max(a.width,a.height)
  }).bind(this))

  for(var i=0; i< sprites.length; i++) {
    var sprite = sprites[i]
    var node = root.insert(sprite.image,this.spacing*2)
    if(!node) return false;
    nodes[i] = node
  }

  var fix = -2

  for(var i=0; i<sprites.length; i++) {
    var node = nodes[i]
    var sprite = sprites[i]
    sprite.coords = {
      width: node.width,
      height: node.height,
      x: node.x,
      y: node.y,
      xmin: (node.x+fix)/this.size,
      ymin: (node.y+fix)/this.size,
      xmax: (node.x+node.width-this.spacing+fix)/this.size,
      ymax: (node.y+node.height-this.spacing+fix)/this.size
    }
  }
  this.sprites = sprites
  this.needUpload = true
  return true
}
SpritesTexture.prototype.tryAddSprite = function(sprite) {
  if(!this.tryPack(this.sprites.concat([sprite]))) {
    return false;
  } else {
    return true;
  }
}
SpritesTexture.prototype.isBigEnough = function(w,h) {
  return (w<this.size && h<this.size)
}
SpritesTexture.prototype.upload = function() {
  if(!this.needUpload) return;

  var ctx = this.ctx
  var canvas = this.canvas
  var gl = this.gl
  ctx.clearRect(0, 0, canvas.width, canvas.height)

  /*ctx.fillStyle="#FF00FF"
  ctx.fillRect(0,0,canvas.width, canvas.height)*/

  for(var i=0; i<this.sprites.length; i++) {
    var sprite = this.sprites[i]
    ctx.drawImage(sprite.image,sprite.coords.x,sprite.coords.y)
  }

  gl.bindTexture(gl.TEXTURE_2D, this.texture)
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, canvas)
  gl.generateMipmap(gl.TEXTURE_2D)

  for(var i=0; i<this.sprites.length; i++) {
    var sprite = this.sprites[i]
    sprite.texture = this.textureId
    //console.log("COORDS",sprite.name,sprite.coords)
    Module._fgfx_Sprite_setTextureFragment(sprite.sprite, sprite.texture,
      sprite.coords.xmin, sprite.coords.ymin, sprite.coords.xmax, sprite.coords.ymax,
      sprite.coords.width, sprite.coords.height, false)
  }

  this.needUpload = false
}
SpritesTexture.prototype.clean = function(engineTime) {
  for(var i=0; i<this.sprites.length; i++) {
    var sprite = this.sprites[i].sprite
    if(sprite && engineTime-sprite.lastUseTime>30) { /// REMOVE SPRITE
      console.log("REMOVE SPRITE",this.sprites[i].name)
      sprite.unload()
      this.sprites.splice(i,1)
      i--;
    }
  }
}

export { SpritesTexture }
export default SpritesTexture