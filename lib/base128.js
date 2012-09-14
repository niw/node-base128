'use strict'

var base128 = module.exports
  , binding = require('../build/Release/base128')

base128.decode = function(string) {
  var buffer = new Buffer(string, 'utf-8')
    , size = ~~(buffer.length * 7 / 8)
    , result = new Buffer(size)
  result.fill(0)
  size = binding.decode(buffer, result)
  return result.slice(0, size)
}

base128.encode = function(buffer) {
  var size = ~~(buffer.length * 8 / 7) + 2
    , result = new Buffer(size)
  result.fill(0)
  size = binding.encode(buffer, result)
  return result.toString('utf-8', 0, size)
}
