#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#define TYPE_ERROR(msg) ThrowException(Exception::TypeError(String::New(msg)));
#define THROW_BAD_ARGS          TYPE_ERROR("Bad argument")
#define THROW_NOT_ENOUGH_BUFFER TYPE_ERROR("Not enough buffer")

static Handle<Value> Decode(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2 ||
     !Buffer::HasInstance(args[0]) ||
     !Buffer::HasInstance(args[1])) {
    return THROW_BAD_ARGS;
  }
  Local<Object> buffer_obj = args[0]->ToObject();
  Local<Object> result_obj = args[1]->ToObject();

  char *data = Buffer::Data(buffer_obj);
  char *res  = Buffer::Data(result_obj);
  size_t data_size = Buffer::Length(buffer_obj);
  size_t res_size  = Buffer::Length(result_obj);

  if(res_size < data_size * 7 / 8) {
    return THROW_NOT_ENOUGH_BUFFER;
  }

  int carry_size = 0;
  unsigned char carry_bits = 0x00;
  size_t i, j;

  for(i = 0, j = 0; i < data_size && j < res_size; ++i) {
    unsigned char c = data[i];
    if(c & 0x80) {
      return THROW_BAD_ARGS;
    }
    c = c & 0x7F;

    if(carry_size == 0) {
      carry_size = 7;
      carry_bits = c;
      continue;
    }

    int mask_size = 8 - carry_size;
    unsigned char mask = (1 << mask_size) - 1;

    res[j++] = carry_bits | ((c & mask) << carry_size);

    carry_bits = c >> mask_size;
    --carry_size;
  }
  if(i < data_size || carry_bits) {
    return THROW_BAD_ARGS;
  }

  return scope.Close(Integer::New(j));
}

static Handle<Value> Encode(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2 ||
     !Buffer::HasInstance(args[0]) ||
     !Buffer::HasInstance(args[1])) {
    return THROW_BAD_ARGS;
  }
  Local<Object> buffer_obj = args[0]->ToObject();
  Local<Object> result_obj = args[1]->ToObject();

  char *data = Buffer::Data(buffer_obj);
  char *res  = Buffer::Data(result_obj);
  size_t data_size = Buffer::Length(buffer_obj);
  size_t res_size  = Buffer::Length(result_obj);

  size_t bits_size = data_size * 8;
  if(res_size < bits_size / 7 + (bits_size % 7 ? 1 : 0)) {
    return THROW_NOT_ENOUGH_BUFFER;
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

    unsigned char c = data[i++];

    int mask_size = 7 - carry_size;
    unsigned char mask = (1 << mask_size) - 1;

    res[j] = ((c & mask) << carry_size) | carry_bits;

    carry_bits = c >> mask_size;
    ++carry_size;
  }
  if(carry_size) {
    res[j++] = carry_bits;
  }

  return scope.Close(Integer::New(j));
}

void init(Handle<Object> target) {
  NODE_SET_METHOD(target, "encode", Encode);
  NODE_SET_METHOD(target, "decode", Decode);
}

NODE_MODULE(base128, init);
