#include <node.h>
#include <nan.h>

#define THROW_BAD_ARGS Nan::ThrowTypeError("Bad argument")
#define THROW_NOT_ENOUGH_BUFFER Nan::ThrowTypeError("Not enough buffer")

NAN_METHOD(Decode) {
  if(info.Length() < 2 ||
     !node::Buffer::HasInstance(info[0]) ||
     !node::Buffer::HasInstance(info[1])) {
    THROW_BAD_ARGS;
    return;
  }
  v8::Local<v8::Object> buffer_obj = info[0]->ToObject();
  v8::Local<v8::Object> result_obj = info[1]->ToObject();

  char * const data = node::Buffer::Data(buffer_obj);
  char * const res = node::Buffer::Data(result_obj);
  const size_t data_size = node::Buffer::Length(buffer_obj);
  const size_t res_size = node::Buffer::Length(result_obj);

  if(res_size < data_size * 7 / 8) {
    THROW_NOT_ENOUGH_BUFFER;
    return;
  }

  int carry_size = 0;
  unsigned char carry_bits = 0x00;
  size_t i, j;

  for(i = 0, j = 0; i < data_size && j < res_size; ++i) {
    unsigned char c = data[i];
    if(c & 0x80) {
      THROW_BAD_ARGS;
      return;
    }
    c = c & 0x7F;

    if(carry_size == 0) {
      carry_size = 7;
      carry_bits = c;
      continue;
    }

    const int mask_size = 8 - carry_size;
    const unsigned char mask = (1 << mask_size) - 1;

    res[j++] = carry_bits | ((c & mask) << carry_size);

    carry_bits = c >> mask_size;
    --carry_size;
  }
  if(i < data_size || carry_bits) {
    THROW_BAD_ARGS;
    return;
  }

  info.GetReturnValue().Set(Nan::New((uint32_t)j));
}

NAN_METHOD(Encode) {
  if(info.Length() < 2 ||
     !node::Buffer::HasInstance(info[0]) ||
     !node::Buffer::HasInstance(info[1])) {
    THROW_BAD_ARGS;
    return;
  }
  v8::Local<v8::Object> buffer_obj = info[0]->ToObject();
  v8::Local<v8::Object> result_obj = info[1]->ToObject();

  char * const data = node::Buffer::Data(buffer_obj);
  char * const res = node::Buffer::Data(result_obj);
  const size_t data_size = node::Buffer::Length(buffer_obj);
  const size_t res_size = node::Buffer::Length(result_obj);

  const size_t bits_size = data_size * 8;
  if(res_size < bits_size / 7 + (bits_size % 7 ? 1 : 0)) {
    THROW_NOT_ENOUGH_BUFFER;
    return;
  }

  int carry_size = 0;
  unsigned char carry_bits = 0x00;
  size_t i, j;

  for(i = 0, j = 0; i < data_size && j < res_size; ++j) {
    if(carry_size == 7) {
      res[j] = carry_bits;
      carry_size = 0;
      carry_bits = 0x00;
      continue;
    }

    const unsigned char c = data[i++];

    const int mask_size = 7 - carry_size;
    const unsigned char mask = (1 << mask_size) - 1;

    res[j] = ((c & mask) << carry_size) | carry_bits;

    carry_bits = c >> mask_size;
    ++carry_size;
  }
  if(carry_size) {
    res[j++] = carry_bits;
  }

  info.GetReturnValue().Set(Nan::New((uint32_t)j));
}

NAN_MODULE_INIT(init) {
  Nan::Export(target, "decode", Decode);
  Nan::Export(target, "encode", Encode);
}

NODE_MODULE(base128, init);
