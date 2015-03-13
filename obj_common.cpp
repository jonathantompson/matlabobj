#include <iostream>
#include <fstream>
#include "mesh.h"
#include "fixed_vector.h"

using namespace Geometry;
using namespace std;

int mapIndex(const int index, const Channel* const channel) {
  if(index < 0) {
    return channel->getSize() + index;
  } else {
    if(index - 1 > channel->getSize() - 1) {
      //LOG_WARNING << index << " and " << channel->getSize();
    }
    return index - 1;
  }
}

void addTri(const int index0, const int index1, const int index2,
  TriChannel* const triChannel, const Channel* const attributeChannel){
  if(index0 == 0 && index1 == 0 && index2 == 0) {
    triChannel->add(Tri(0, 0, 0));
  } else {
    const int mappedIndex0 = mapIndex(index0, attributeChannel);
    const int mappedIndex1 = mapIndex(index1, attributeChannel);
    const int mappedIndex2 = mapIndex(index2, attributeChannel);
    triChannel->add(Tri(mappedIndex0, mappedIndex1, mappedIndex2));
  }
}

typedef FixedVector<int, 16> Indices;

int stringToInt(const string& s){
	return atoi(s.c_str());
}

int stringToIndex(const string& s) {
  if(s == "") return 0;
  //const int index = StringTools::stringToInt(s);
  return stringToInt(s);
}

// From http://www.digitalpeer.com/id/simple
template<int maxTokenCount> static void fastTokenize(const std::string& str,
  const std::string& delimiters, FixedVector<std::string, maxTokenCount>& tokens) {
  
  // skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  
  // find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  
  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    
    // skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    
    // find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void getIndicesFromLine(const char* const line, Indices& positionIndices,
  Indices& normalIndices, Indices& texCoordIndices) {
  FixedVector<string, 16> vertexTokens;
  fastTokenize(line + 2, " \t", vertexTokens);
  
  const int vertexTokenCount = int(vertexTokens.size());
  for(int i = 0; i < vertexTokenCount; i++) {
    const string& vertexToken = vertexTokens[i];
    
    FixedVector<string, 16> componentTokens;
    fastTokenize(vertexToken, "/", componentTokens);
    const int componentCount = int(componentTokens.size());
    
    if(componentCount == 1) {
      positionIndices.push_back(stringToIndex(componentTokens[0]));
      normalIndices.push_back(0);
      texCoordIndices.push_back(0);
    } else if(componentCount == 2) {
      positionIndices.push_back(stringToIndex(componentTokens[0]));
      normalIndices.push_back(stringToIndex(componentTokens[1]));
      texCoordIndices.push_back(0);
    } else if(componentCount == 3) {
      positionIndices.push_back(stringToIndex(componentTokens[0]));
      normalIndices.push_back(stringToIndex(componentTokens[1]));
      texCoordIndices.push_back(stringToIndex(componentTokens[2]));
    } else {
      throw runtime_error("Unsupported face format");
    }
  }
}

void loadFromOBJFile(const std::string& filename, Mesh* const mesh) {
  
  mesh->clear();
  
  Vec3fChannel* const positionChannel = new Vec3fChannel("Position", mesh);
  Vec3fChannel* const normalChannel = new Vec3fChannel("Normal", mesh);
  Vec2fChannel* const texCoordChannel = new Vec2fChannel("TexCoord", mesh);
  TriChannel* const positionTriChannel = new TriChannel("Tri", mesh);
  TriChannel* const normalTriChannel = new TriChannel("Normal Tri", mesh);
  TriChannel* const texCoordTriChannel = new TriChannel("TexCoord Tri", mesh);
  IntChannel* const materialIdChannel = new IntChannel("MaterialId", mesh);
  
  fstream stream;
  stream.open(filename.c_str(), ios_base::in | ios_base::binary);
  
  if(!stream.is_open()) {
    throw runtime_error("File not found while reading OBJ file: '" + filename + "'");
  }
  
  map<string, int> previousMaterials;
  int currentMaterialId = -1;
  
  int lineIndex = 0;
  while(!stream.eof()) {
    const int maxLineLength = 512;
    
    char brokenLine[maxLineLength];
    stream.getline(brokenLine, maxLineLength);
    
    char line[4 * maxLineLength];
    int unbreakIndexIn = 0;
    int unbreakIndexOut = 0;
    while(brokenLine[unbreakIndexIn] != 0 && unbreakIndexIn < maxLineLength) {
      if(	brokenLine[unbreakIndexIn + 0] == '/' &&
              brokenLine[unbreakIndexIn + 1] == '/')
      {
        line[unbreakIndexOut++] = '/';
        line[unbreakIndexOut++] = '1';
        unbreakIndexIn++;
      } else {
        line[unbreakIndexOut++] = brokenLine[unbreakIndexIn++];
      }
    }
    line[unbreakIndexOut] = 0;
    
    if(unbreakIndexOut != unbreakIndexIn) {
      //LOG_INFO << "Fixed line: " << line;
    }
    
    // Concatenate "/"'s on the end
    while(line[strlen(line) - 2] == '\\') {
      char nextLine[maxLineLength];
      stream.getline(nextLine, maxLineLength);
      line[strlen(line) - 2] = 0;
      strcat(line, " ");
      strcat(line, nextLine);
      //LOG_INFO << "Concatenating " << lineIndex << " to be '" << line << "'";
      lineIndex++;
    }
    
    // Remove trailing whitespaces
    while(true) {
      const int endIndex = int(strlen(line)) - 2;
      if(endIndex > 0 && (line[endIndex] == ' ' || line[endIndex] == '\t')) {
        line[endIndex] = 0;
      } else {
        break;
      }
    }
    
    char elementID[256];
    sscanf(line, "%s", elementID);
    
    if(	line[0] == 'o'||
            line[0] == 13 ||
            line[0] == ' ' ||
            line[0] == '\0' ||
            line[0] == '#' ||
            line[0] == 's')
    {
      // Those are things we ignore, just go on ..
    } else if(!strcmp(elementID, "v")) {
      float x, y, z;
      sscanf(line, "v %f %f %f", &x, &y, &z);
      positionChannel->add(Vec3f(x, y, z));
    } else if(!strcmp(elementID, "vn")) {
      float nx, ny, nz;
      sscanf(line, "vn %f %f %f", &nx, &ny, &nz);
      normalChannel->add(Vec3f(nx, ny, nz));
    } else if(!strcmp(elementID, "vt")) {
      float s, t;
      sscanf(line, "vt %f %f", &s, &t);
      texCoordChannel->add(Vec2f(s, t));
    } else if(!strcmp(elementID, "f")) {
      
      Indices positionIndices;
      Indices normalIndices;
      Indices texCoordIndices;
      getIndicesFromLine(line, positionIndices, normalIndices, texCoordIndices);
      
      const int indexCount = int(positionIndices.size());
      const int triCount = indexCount - 2;
      
      for(int i = 0; i < triCount; i++) {
        const int positionIndex0 = positionIndices[0];
        const int positionIndex1 = positionIndices[i + 1];
        const int positionIndex2 = positionIndices[i + 2];
        const int texCoordIndex0 = normalIndices[0];
        const int texCoordIndex1 = normalIndices[i + 1];
        const int texCoordIndex2 = normalIndices[i + 2];
        const int normalIndex0 = texCoordIndices[0];
        const int normalIndex1 = texCoordIndices[i + 1];
        const int normalIndex2 = texCoordIndices[i + 2];
        
        addTri(positionIndex0, positionIndex1, positionIndex2, positionTriChannel, positionChannel);
        addTri(texCoordIndex0, texCoordIndex1, texCoordIndex2, texCoordTriChannel, texCoordChannel);
        addTri(normalIndex0, normalIndex1, normalIndex2, normalTriChannel, normalChannel);
        materialIdChannel->add(currentMaterialId);
      }
      
    } else if(!strcmp(elementID, "mtllib")) {
      // This is a material definition -- ignore it
    } else if(!strcmp(elementID, "usemtl")) {
      
      char materialName[256];
      sscanf(line, "usemtl %s", materialName);
      
      const map<string, int>::iterator search = previousMaterials.find(materialName);
      if(search == previousMaterials.end()) {
        currentMaterialId = int(previousMaterials.size());
        //LOG_INFO << "Creating new material '" << materialName << "' mapped to id " << currentMaterialId;
        previousMaterials[materialName] = currentMaterialId;
      } else {
        currentMaterialId = search->second;
        //LOG_INFO << "Re-using material named '" << materialName << "' mapped to id " << currentMaterialId;
      }
    } else if(!strcmp(elementID, "g")) {
    } else {
      throw runtime_error("Unsupported element in line '" + string(line) + "'");
    }
    
    lineIndex++;
  }
  
  // Would it make things simpler to cull tri channel that are the same as the position tri channel?
  // Simply go over those chanels, and if the same, to the manual replace
  
  if(positionChannel->getSize()) {
    mesh->addChannel(positionChannel);
    mesh->addChannel(positionTriChannel);
    mesh->addRealization(positionChannel, positionTriChannel);
  }
  if(normalChannel->getSize()) {
    // Number of normal tris must be the same as the number of position tri
    assert(normalTriChannel->getSize() == positionTriChannel->getSize());
    
    mesh->addChannel(normalChannel);
    mesh->addChannel(normalTriChannel);
    mesh->addRealization(normalChannel, normalTriChannel);
  }
  if(texCoordChannel->getSize()) {
    // Number of texcoord tris must be the same as the number of position tri
    assert(texCoordTriChannel->getSize() == positionTriChannel->getSize());
    mesh->addChannel(texCoordChannel);
    mesh->addChannel(texCoordTriChannel);
    mesh->addRealization(texCoordChannel, texCoordTriChannel);
  }
  
  // Add the material channel, but only if it is non-trivial, e.g. contains more than one value
  if(previousMaterials.size() > 1) {
    mesh->addChannel(materialIdChannel);
    FlatChannel* const materialIdTriChannel = new FlatChannel("MaterialId Tri", mesh);
    mesh->addChannel(materialIdTriChannel);
    mesh->addRealization(materialIdChannel, materialIdTriChannel);
  }
}
