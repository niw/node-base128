'use strict'

var should = require('should')
  , base128 = require('../lib/base128')

describe('base128', function() {
  describe('decode', function() {
    it('should throw Bad argument error with U+0000', function() {
      !function() {
        base128.decode('\u0000')
      }.should.throw(/Bad argument/)
    })

    it('should throw Bad argument error with U+0080', function() {
      !function() {
        base128.decode('\u0080')
      }.should.throw(/Bad argument/)
    })

    it('should throw Bad argument error with U+007F, U+0003', function() {
      !function() {
        base128.decode('\u007F\u0003')
      }.should.throw(/Bad argument/)
    })

    it('should throw Bad argument error with U+007F, U+007F, U+007F, U+007F, U+007F, U+007F, U+007F, U+007F, U+0000', function() {
      !function() {
        base128.decode('\u007f\u007f\u007f\u007f\u007f\u007f\u007f\u007f\u0000')
      }.should.throw(/Bad argument/)
    })

    it('should decode blank string', function() {
      var buf = base128.decode('')
      buf.length.should.equal(0)
    })

    it('should decode U+007F, U+0001', function() {
      var buf = base128.decode('\u007F\u0001')
      buf.length.should.equal(1)
      buf[0].should.equal(0xFF)
    })

    it('should decode U+007F, U+0003, U+0000', function() {
      var buf = base128.decode('\u007F\u0003\u0000')
      buf.length.should.equal(2)
      buf[0].should.equal(0xFF)
      buf[1].should.equal(0x01)
    })

    it('should decode U+007F, U+007F, U+007F, U+007F, U+007F, U+007F, U+007F, U+007F', function() {
      var buf = base128.decode('\u007f\u007f\u007f\u007f\u007f\u007f\u007f\u007f')
      buf.length.should.equal(7)
      for(var i=0; i<7; i++) {
        buf[i].should.equal(0xFF)
      }
    })
  })

  describe('encode', function() {
    it('should encode blank', function() {
      base128.encode(new Buffer([])).should.equal("")
    })

    it('should encode 0x00', function() {
      base128.encode(new Buffer([0x00])).should.equal("\u0000\u0000")
    })

    it('should encode 0xFF, 0x01', function() {
      base128.encode(new Buffer([0xFF,0x01])).should.equal("\u007F\u0003\u0000")
    })

    it('should encode 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF', function() {
      base128.encode(new Buffer([0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]))
        .should.equal("\u007F\u007F\u007F\u007F\u007F\u007F\u007F\u007F")
    })
  })
})
