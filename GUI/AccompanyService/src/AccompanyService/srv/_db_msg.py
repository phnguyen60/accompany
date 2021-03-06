"""autogenerated by genmsg_py from db_msgRequest.msg. Do not edit."""
import roslib.message
import struct


class db_msgRequest(roslib.message.Message):
  _md5sum = "181bdb76a87c5a550e92c7efea63a2fa"
  _type = "AccompanyService/db_msgRequest"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """int64 request
string param
int64 sonReq

"""
  __slots__ = ['request','param','sonReq']
  _slot_types = ['int64','string','int64']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       request,param,sonReq
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(db_msgRequest, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.request is None:
        self.request = 0
      if self.param is None:
        self.param = ''
      if self.sonReq is None:
        self.sonReq = 0
    else:
      self.request = 0
      self.param = ''
      self.sonReq = 0

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    @param buff: buffer
    @type  buff: StringIO
    """
    try:
      buff.write(_struct_q.pack(self.request))
      _x = self.param
      length = len(_x)
      buff.write(struct.pack('<I%ss'%length, length, _x))
      buff.write(_struct_q.pack(self.sonReq))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    @param str: byte array of serialized message
    @type  str: str
    """
    try:
      end = 0
      start = end
      end += 8
      (self.request,) = _struct_q.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      self.param = str[start:end]
      start = end
      end += 8
      (self.sonReq,) = _struct_q.unpack(str[start:end])
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    @param buff: buffer
    @type  buff: StringIO
    @param numpy: numpy python module
    @type  numpy module
    """
    try:
      buff.write(_struct_q.pack(self.request))
      _x = self.param
      length = len(_x)
      buff.write(struct.pack('<I%ss'%length, length, _x))
      buff.write(_struct_q.pack(self.sonReq))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    @param str: byte array of serialized message
    @type  str: str
    @param numpy: numpy python module
    @type  numpy: module
    """
    try:
      end = 0
      start = end
      end += 8
      (self.request,) = _struct_q.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      self.param = str[start:end]
      start = end
      end += 8
      (self.sonReq,) = _struct_q.unpack(str[start:end])
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_q = struct.Struct("<q")
"""autogenerated by genmsg_py from db_msgResponse.msg. Do not edit."""
import roslib.message
import struct


class db_msgResponse(roslib.message.Message):
  _md5sum = "1d865bd8b4398a7d5fd16ee78aa4168b"
  _type = "AccompanyService/db_msgResponse"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """int64 code
int64 sonRes
string answer



"""
  __slots__ = ['code','sonRes','answer']
  _slot_types = ['int64','int64','string']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       code,sonRes,answer
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(db_msgResponse, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.code is None:
        self.code = 0
      if self.sonRes is None:
        self.sonRes = 0
      if self.answer is None:
        self.answer = ''
    else:
      self.code = 0
      self.sonRes = 0
      self.answer = ''

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    @param buff: buffer
    @type  buff: StringIO
    """
    try:
      _x = self
      buff.write(_struct_2q.pack(_x.code, _x.sonRes))
      _x = self.answer
      length = len(_x)
      buff.write(struct.pack('<I%ss'%length, length, _x))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    @param str: byte array of serialized message
    @type  str: str
    """
    try:
      end = 0
      _x = self
      start = end
      end += 16
      (_x.code, _x.sonRes,) = _struct_2q.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      self.answer = str[start:end]
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    @param buff: buffer
    @type  buff: StringIO
    @param numpy: numpy python module
    @type  numpy module
    """
    try:
      _x = self
      buff.write(_struct_2q.pack(_x.code, _x.sonRes))
      _x = self.answer
      length = len(_x)
      buff.write(struct.pack('<I%ss'%length, length, _x))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    @param str: byte array of serialized message
    @type  str: str
    @param numpy: numpy python module
    @type  numpy: module
    """
    try:
      end = 0
      _x = self
      start = end
      end += 16
      (_x.code, _x.sonRes,) = _struct_2q.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      self.answer = str[start:end]
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_2q = struct.Struct("<2q")
class db_msg(roslib.message.ServiceDefinition):
  _type          = 'AccompanyService/db_msg'
  _md5sum = '3e6bed7dbd53be1f4d5a46e4a74c1074'
  _request_class  = db_msgRequest
  _response_class = db_msgResponse
