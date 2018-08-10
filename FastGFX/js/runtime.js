import { TextureManager } from "./TextureManager.js"
import { Animator } from "./Animator.js"

var canvas = document.getElementById('fgfx_canvas')
var glAttrs = {
  alpha: true
}
var gl = canvas.getContext('webgl', glAttrs) || canvas.getContext('experimental-webgl', glAttrs)

var animator = new Animator(false)

var textureManager

var loadImage = function(url) {
  return new Promise((resolve,reject) => {
    var image = new Image()
    image.crossOrigin = "anonymous"
    image.src = url
    if(image.complete) {
      return resolve(image)
    }
    image.onload=function() {
      resolve(image)
    }
    image.onerror=function() {
      reject('Could not load image '+url)
    }
  })
}

var loadText = function(url) {
  return new Promise((resolve, reject) => {
    var http = new XMLHttpRequest()
    http.requestType = "text"
    http.open('GET', url, true)
    http.onreadystatechange = function () {
      if (http.readyState == 4) {
        if (http.status == 200) {
          resolve(http.responseText)
        } else {
          reject('HTTP download error on url ' + url + ' STATUS: ' + http.status + ': ' + http.statusText)
        }
      }
    }
    http.send(null)
  })
}

window.fgfxLoader = window.fgfxLoader || {
    loadSpriteImage: loadImage,
    loadTextureImage: loadImage,
    loadFontData: (name) => loadText(name + '.fnt'),
    loadFontImage: (name) => loadImage(name + '.png'),
    loadFile: (name) => loadText(name)
  }

window.Module = window.Module || {}
window.Module.initializeFastGfxRuntime = function() {
  console.log("INIT!")
  GLctx = gl
  animator.addAnimation(animateFgfx)
}
window.Module.initializeFastGfxTextureManager = function() {
  textureManager = new TextureManager(gl,window.fgfxLoader)
}

var pixelRatio = window.devicePixelRatio || 1
var animateFgfx = function(time,delta) {
  textureManager.reload()
  GLctx = gl

  if( canvas.width != canvas.clientWidth * pixelRatio || canvas.height != canvas.clientHeight * pixelRatio || (window.devicePixelRatio || 1) != pixelRatio) {
    pixelRatio = window.devicePixelRatio || 1
    canvas.width = canvas.clientWidth * pixelRatio
    canvas.height = canvas.clientHeight * pixelRatio
    gl.viewport( 0, 0, canvas.width, canvas.height)
    console.log("CANVAS RESIZE", canvas.width, canvas.height, pixelRatio)
  }
  window.Module._fgfx_render(time, delta, canvas.width, canvas.height, canvas.clientWidth * 0.264, canvas.clientHeight * 0.264)
}
