#include <stdio.h>
#include <string>
#include "obj_common.h"
#include "mex.h"

using namespace Geometry;
using namespace std;

// The gateway function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  // Only 1 input allowed
  if (nrhs != 1) {
    mexErrMsgIdAndTxt("MATLAB:obj_read:invalidNumInputs", 
      "Specify filename.");
  }
  // input must be a string
  if (mxIsChar(prhs[0]) != 1) {
    mexErrMsgIdAndTxt("MATLAB:obj_read:inputNotString",
      "Input must be a string.");
  }
  // copy the string data from prhs[0] into a C string input_ buf.
  char *filename = mxArrayToString(prhs[0]);
  if (filename == NULL) {
    mexErrMsgIdAndTxt("MATLAB:obj_read:conversionFailed",
      "Could not convert input to string.");
  }
  // Only 1 input allowed
  if (nlhs != 1) {
    mexErrMsgIdAndTxt("MATLAB:obj_read:invalidNumOutputs", 
      "One output is required");
  }
  
  Mesh* mesh = new Mesh();
  loadFromOBJFile(filename, mesh);
  
  int num_fields = (int)mesh->getChannels().size();
  
  const char** field_names;  // Field names for the return struct
  field_names = (const char**)mxCalloc(num_fields, sizeof(*field_names));
  
  for (int i = 0; i < num_fields; i++) {
	  const Channel* const channel = mesh->getChannels()[i];
    field_names[i] = channel->getName().c_str();
  }
  
  plhs[0] = mxCreateStructMatrix(1, 1, num_fields, field_names);
  
  for (int ifield = 0; ifield < num_fields; ifield++) {
    const Channel* const channel = mesh->getChannels()[ifield];
    const int channelMemoryUsage = channel->getMemoryUsage();
    const int size = channel->getSize();
    
    // We don't know what kind of channel it is so just try them all :-(
    const IntChannel* const intChannel = dynamic_cast<const IntChannel*>(channel);
    const FloatChannel* const floatChannel = dynamic_cast<const FloatChannel*>(channel);
    const Vec2fChannel* const vec2fChannel = dynamic_cast<const Vec2fChannel*>(channel);
    const Vec3fChannel* const vec3fChannel = dynamic_cast<const Vec3fChannel*>(channel);
    const Vec4fChannel* const vec4fChannel = dynamic_cast<const Vec4fChannel*>(channel);
    const EdgeChannel* const edgeChannel = dynamic_cast<const EdgeChannel*>(channel);
    const TriChannel* const triChannel = dynamic_cast<const TriChannel*>(channel);
    const TetraChannel* const tetraChannel = dynamic_cast<const TetraChannel*>(channel);
    
    int dims = 0;
    if (intChannel) {
      dims = 1;
    } else if (floatChannel) {
      dims = 1;
    } else if (vec2fChannel) {
      dims = 2;
    } else if (vec3fChannel) {
      dims = 3;
    } else if (vec4fChannel) {
      dims = 4;
    } else if (edgeChannel) {
      dims = 2;
    } else if (triChannel) {
      dims = 3;
    } else if (tetraChannel) {
      dims = 4;
    }
    
    // Make all the return values doubles (even indices)
    mxArray* fout = mxCreateDoubleMatrix(dims, size, mxREAL);
    double* fout_data = mxGetPr(fout);
    mxSetFieldByNumber(plhs[0], 0, ifield, fout);
    
    if (intChannel) {
      const std::vector<int>& values = intChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i] = static_cast<double>(values[i]);
      }
    } else if (floatChannel) {
      const std::vector<float>& values = floatChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i] = static_cast<double>(values[i]);
      }
    } else if (vec2fChannel) {
      const std::vector<Vec2f>& values = vec2fChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*2+0] = static_cast<double>(values[i].x);
        fout_data[i*2+1] = static_cast<double>(values[i].y);
      }
    } else if (vec3fChannel) {
      const std::vector<Vec3f>& values = vec3fChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*3+0] = static_cast<double>(values[i].x);
        fout_data[i*3+1] = static_cast<double>(values[i].y);
        fout_data[i*3+2] = static_cast<double>(values[i].z);
      }
    } else if (vec4fChannel) {
      const std::vector<Vec4f>& values = vec4fChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*4+0] = static_cast<double>(values[i].x);
        fout_data[i*4+1] = static_cast<double>(values[i].y);
        fout_data[i*4+2] = static_cast<double>(values[i].z);
        fout_data[i*4+3] = static_cast<double>(values[i].w);
      }
    } else if (edgeChannel) {
      const std::vector<Edge>& values = edgeChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*2+0] = static_cast<double>(values[i].indices[0]);
        fout_data[i*2+1] = static_cast<double>(values[i].indices[1]);
      }
    } else if (triChannel) {
      const std::vector<Tri>& values = triChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*3+0] = static_cast<double>(values[i].indices[0]);
        fout_data[i*3+1] = static_cast<double>(values[i].indices[1]);
        fout_data[i*3+2] = static_cast<double>(values[i].indices[2]);
      }
    } else if (tetraChannel) {
      const std::vector<Tetra>& values = tetraChannel->getValues();
      for (int i = 0; i < size; i++) {
        fout_data[i*4+0] = static_cast<double>(values[i].indices[0]);
        fout_data[i*4+1] = static_cast<double>(values[i].indices[1]);
        fout_data[i*4+2] = static_cast<double>(values[i].indices[2]);
        fout_data[i*4+3] = static_cast<double>(values[i].indices[3]);
      }
    }
  }
  
  mxFree((void *)field_names);
}