#ifndef __DEF_H__
#define __DEF_H__


enum status {
  ST_OK = 0,
  ST_BAD_FORMAT = 1,
  ST_BAD_REQUEST = 2,
  ST_BAD_VALUE = 3,
  ST_HW_ERROR = 4,
  ST_NOT_SUPPORTED = 5,
  ST_NOT_IMPLEMENTED = 6,
  ST_DOES_NOT_EXIST = 7,
  ST_ALREADY_EXISTS = 8,
  ST_BUSY = 9,
  ST_NOT_READY = 10,
  ST_BAD_STATE = 11,
  ST_REMOTE = 12,
  ST_HEX = 13,
  ST_MALLOC_ERROR = 14
};


#endif
