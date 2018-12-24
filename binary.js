/* Copyright (C) Silvertone Electronics - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Sam Cowen <sam.cowen@silvertone.com.au>, August 2017
 */

/**
 * The 'binary' module contains an assortment of functions used to convert integers and floats
 * to and from byte arrays. Byte order is least-significant-byte first.
 */

'use strict';

module.exports = class Binary {
  // 32bit signed int to byte array
  int32ToBytes (num) {
    var arr = new ArrayBuffer(4);
    var view = new DataView(arr);
    view.setInt32(0, num, true);
    var ret = [];
    for (var i = 0; i < 4; i++) {
      ret = ret.concat(view.getUint8(i));
    }
    return ret;
  }

  // byte array to 32bit signed int
  // offset sets the position of the integer in the array
  bytesToInt32 (bytes, offset) {
    var u8 = new Uint8Array(bytes);
    var view = new DataView(u8.buffer);
    return view.getInt32(offset, true);
  }

  // 32bit unsigned int to bytes
  uint32ToBytes (num) {
    var arr = new ArrayBuffer(4);
    var view = new DataView(arr);
    view.setUint32(0, num, true);
    var ret = [];
    for (var i = 0; i < 4; i++) {
      ret = ret.concat(view.getUint8(i));
    }
    return ret;
  }

  bytesToUint32 (bytes, offset) {
    var u8 = new Uint8Array(bytes);
    var view = new DataView(u8.buffer);
    return view.getUint32(offset, true);
  }

  int16ToBytes (num) {
    var arr = new ArrayBuffer(2);
    var view = new DataView(arr);
    view.setInt16(0, num, true);
    var ret = [];
    for (var i = 0; i < 2; i++) {
      ret = ret.concat(view.getUint8(i));
    }
    return ret;
  }

  bytesToInt16 (bytes, offset) {
    var u8 = new Uint8Array(bytes);
    var view = new DataView(u8.buffer);
    return view.getInt16(offset, true);
  }

  uint16ToBytes (num) {
    var arr = new ArrayBuffer(2);
    var view = new DataView(arr);
    view.setUint16(0, num, true);
    var ret = [];
    for (var i = 0; i < 2; i++) {
      ret = ret.concat(view.getUint8(i));
    }
    return ret;
  }

  bytesToUint16 (bytes, offset) {
    var u8 = new Uint8Array(bytes);
    var view = new DataView(u8.buffer);
    return view.getUint16(offset, true);
  }

  floatToBytes (num) {
    var arr = new ArrayBuffer(4);
    var view = new DataView(arr);
    view.setFloat32(0, num, true);
    var ret = [];
    for (var i = 0; i < 4; i++) {
      ret = ret.concat(view.getUint8(i));
    }
    return ret;
  }

  bytesToFloat (bytes, offset) {
    var u8 = new Uint8Array(bytes);
    var view = new DataView(u8.buffer);
    return view.getFloat32(offset, true);
  }

  bytesToString (array) {
    var result = '';
    for (var i = 0; i < array.length; ++i) {
      result += (String.fromCharCode(array[i]));
    }
    return result;
  }

  stringToBytes (str) {
    var result = [];
    for (var i = 0; i < str.length; i++) {
      result.push(str.charCodeAt(i));
    }
    return result;
  }
};
