/*
 * RAF fallbacks
 */
const requestAnimationFrame = window.requestAnimationFrame || window.webkitRequestAnimationFrame ||
  window.mozRequestAnimationFrame ||
  function(  callback, element ) {
    return window.setTimeout( callback, 1000 / 60 )
  }

const cancelAnimationFrame = window.cancelAnimationFrame || window.mozCancelAnimationFrame || window.clearTimeout

/*
 * now
 */
let nowOffset = Date.now()
let now = () => Date.now() - nowOffset
if(window.performance && window.performance.now) {
  if (window.performance.timing && window.performance.timing.navigationStart){
    nowOffset = window.performance.timing.navigationStart
  }
  now = () => window.performance.now()
}
/**
 * Constructs animator
 * @constructor
 */
var Animator = function(lagAnimation) {
  this.animations = []
  this.lastAnimatedObjectId = 0
  this.startTime = now()
  this.lastFrameTime = this.startTime
  this.lastFramesTimes = [this.startTime]
  this.animate()

  this.frameTime = now()

  if(lagAnimation) setInterval(this.lagAnimate.bind(this), 200)

  this.lastRaf = 0
}
Animator.prototype.addAnimation = function(animation) {
  this.animations.push(animation)
}
Animator.prototype.animate = function() {
  var frameTime = this.getCurrentTime() // Current frame time
  var delta = frameTime - this.lastFrameTime // Calculates delta, useful for some animations
  for(let animation of this.animations) {
    animation(frameTime, delta)
  }
  this.lastFrameTime=frameTime
  if(!this.raf) this.raf=window.requestAnimationFrame(this.rafAnimate.bind(this))
}
Animator.prototype.rafAnimate = function(time) {
  //console.error("RAF ANIMATE!")
  this.frameTime = now()
  this.raf = null
  this.lastRaf = this.frameTime
  this.animate()
}
Animator.prototype.lagAnimate = function() {
  var ctime = now()
  if((ctime - this.lastRaf) > 200) {
    //console.error("LAG ANIMATE",ctime,this.lastRaf,ctime-this.lastRaf)
    this.lastRaf = ctime
    this.frameTime = ctime
    this.animate()
  }
}
Animator.prototype.getCurrentTime = function() {
  return this.frameTime
}

export {Animator, now}
export default Animator