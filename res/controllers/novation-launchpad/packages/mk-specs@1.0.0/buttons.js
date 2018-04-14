'use strict'
module.exports = {
  /* eslint-disable key-spacing, no-multi-spaces */
  'up':      { status: '0xB0', midino: '0x68', name: 'up' },
  'down':    { status: '0xB0', midino: '0x69', name: 'down' },
  'left':    { status: '0xB0', midino: '0x6A', name: 'left' },
  'right':   { status: '0xB0', midino: '0x6B', name: 'right' },
  'session': { status: '0xB0', midino: '0x6C', name: 'session' },
  'user1':   { status: '0xB0', midino: '0x6D', name: 'user1' },
  'user2':   { status: '0xB0', midino: '0x6E', name: 'user2' },
  'mixer':   { status: '0xB0', midino: '0x6F', name: 'mixer' },
  'vol':     { status: '0x90', midino: '0x08', name: 'vol' },
  'pan':     { status: '0x90', midino: '0x18', name: 'pan' },
  'snda':    { status: '0x90', midino: '0x28', name: 'snda' },
  'sndb':    { status: '0x90', midino: '0x38', name: 'sndb' },
  'stop':    { status: '0x90', midino: '0x48', name: 'stop' },
  'trkon':   { status: '0x90', midino: '0x58', name: 'trkon' },
  'solo':    { status: '0x90', midino: '0x68', name: 'solo' },
  'arm':     { status: '0x90', midino: '0x78', name: 'arm' },
  '0,0':     { status: '0x90', midino: '0x00', name: '0,0' },
  '0,1':     { status: '0x90', midino: '0x01', name: '0,1' },
  '0,2':     { status: '0x90', midino: '0x02', name: '0,2' },
  '0,3':     { status: '0x90', midino: '0x03', name: '0,3' },
  '0,4':     { status: '0x90', midino: '0x04', name: '0,4' },
  '0,5':     { status: '0x90', midino: '0x05', name: '0,5' },
  '0,6':     { status: '0x90', midino: '0x06', name: '0,6' },
  '0,7':     { status: '0x90', midino: '0x07', name: '0,7' },
  '1,0':     { status: '0x90', midino: '0x10', name: '1,0' },
  '1,1':     { status: '0x90', midino: '0x11', name: '1,1' },
  '1,2':     { status: '0x90', midino: '0x12', name: '1,2' },
  '1,3':     { status: '0x90', midino: '0x13', name: '1,3' },
  '1,4':     { status: '0x90', midino: '0x14', name: '1,4' },
  '1,5':     { status: '0x90', midino: '0x15', name: '1,5' },
  '1,6':     { status: '0x90', midino: '0x16', name: '1,6' },
  '1,7':     { status: '0x90', midino: '0x17', name: '1,7' },
  '2,0':     { status: '0x90', midino: '0x20', name: '2,0' },
  '2,1':     { status: '0x90', midino: '0x21', name: '2,1' },
  '2,2':     { status: '0x90', midino: '0x22', name: '2,2' },
  '2,3':     { status: '0x90', midino: '0x23', name: '2,3' },
  '2,4':     { status: '0x90', midino: '0x24', name: '2,4' },
  '2,5':     { status: '0x90', midino: '0x25', name: '2,5' },
  '2,6':     { status: '0x90', midino: '0x26', name: '2,6' },
  '2,7':     { status: '0x90', midino: '0x27', name: '2,7' },
  '3,0':     { status: '0x90', midino: '0x30', name: '3,0' },
  '3,1':     { status: '0x90', midino: '0x31', name: '3,1' },
  '3,2':     { status: '0x90', midino: '0x32', name: '3,2' },
  '3,3':     { status: '0x90', midino: '0x33', name: '3,3' },
  '3,4':     { status: '0x90', midino: '0x34', name: '3,4' },
  '3,5':     { status: '0x90', midino: '0x35', name: '3,5' },
  '3,6':     { status: '0x90', midino: '0x36', name: '3,6' },
  '3,7':     { status: '0x90', midino: '0x37', name: '3,7' },
  '4,0':     { status: '0x90', midino: '0x40', name: '4,0' },
  '4,1':     { status: '0x90', midino: '0x41', name: '4,1' },
  '4,2':     { status: '0x90', midino: '0x42', name: '4,2' },
  '4,3':     { status: '0x90', midino: '0x43', name: '4,3' },
  '4,4':     { status: '0x90', midino: '0x44', name: '4,4' },
  '4,5':     { status: '0x90', midino: '0x45', name: '4,5' },
  '4,6':     { status: '0x90', midino: '0x46', name: '4,6' },
  '4,7':     { status: '0x90', midino: '0x47', name: '4,7' },
  '5,0':     { status: '0x90', midino: '0x50', name: '5,0' },
  '5,1':     { status: '0x90', midino: '0x51', name: '5,1' },
  '5,2':     { status: '0x90', midino: '0x52', name: '5,2' },
  '5,3':     { status: '0x90', midino: '0x53', name: '5,3' },
  '5,4':     { status: '0x90', midino: '0x54', name: '5,4' },
  '5,5':     { status: '0x90', midino: '0x55', name: '5,5' },
  '5,6':     { status: '0x90', midino: '0x56', name: '5,6' },
  '5,7':     { status: '0x90', midino: '0x57', name: '5,7' },
  '6,0':     { status: '0x90', midino: '0x60', name: '6,0' },
  '6,1':     { status: '0x90', midino: '0x61', name: '6,1' },
  '6,2':     { status: '0x90', midino: '0x62', name: '6,2' },
  '6,3':     { status: '0x90', midino: '0x63', name: '6,3' },
  '6,4':     { status: '0x90', midino: '0x64', name: '6,4' },
  '6,5':     { status: '0x90', midino: '0x65', name: '6,5' },
  '6,6':     { status: '0x90', midino: '0x66', name: '6,6' },
  '6,7':     { status: '0x90', midino: '0x67', name: '6,7' },
  '7,0':     { status: '0x90', midino: '0x70', name: '7,0' },
  '7,1':     { status: '0x90', midino: '0x71', name: '7,1' },
  '7,2':     { status: '0x90', midino: '0x72', name: '7,2' },
  '7,3':     { status: '0x90', midino: '0x73', name: '7,3' },
  '7,4':     { status: '0x90', midino: '0x74', name: '7,4' },
  '7,5':     { status: '0x90', midino: '0x75', name: '7,5' },
  '7,6':     { status: '0x90', midino: '0x76', name: '7,6' },
  '7,7':     { status: '0x90', midino: '0x77', name: '7,7' }
  /* eslint-enable key-spacing, no-multi-spaces */
}
