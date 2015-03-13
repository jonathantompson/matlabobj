#pragma once

#include <string>
#include <vector>
#include <map>

#include "channel.h"

namespace Geometry {
	
	class Mesh {

	private:

		// The channels themself
		std::vector<Channel*> channels;

		typedef std::multimap<Channel*, Channel*> Realization;
		
		// One complex channels maps to multiple value channels
		Realization realization;
		
	public:

		~Mesh() {
			clear();
		}

		void clear() {
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount; i++) {
				if(channels[i]->getOwner() == this) {
					delete channels[i];
				}
			}
			channels.clear();
			realization.clear();
		}

		void makeInstanceOf(const Mesh* const mesh) {
			clear();

			channels = mesh->channels;
			realization = mesh->realization;
		}

		void addChannel(Channel* const channel) {
			channels.push_back(channel);
		}

		void removeChannel(Channel* const channel) {
			// Poor mans delete!
			const std::vector<Channel*> oldChannels = channels;			
			const int channelCount = int(channels.size());
			channels.clear();
			for(int i = 0; i < channelCount; i++) {
				Channel* const oldChannel = oldChannels[i];
				if(oldChannel != channel) {
					channels.push_back(oldChannel);
				}
			}
		}

		std::vector<Channel*>& getChannels() {
			return channels;
		}
		
		const std::vector<Channel*>& getChannels() const {
			return channels;
		}

		Channel* getChannelByName(const std::string& name) {
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount ; i++) {
				if(channels[i]->getName() == name) {
					return channels[i];
				}
			}			
			return NULL;
		}

		const Channel* getChannelByName(const std::string& name) const {
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount ; i++) {
				if(channels[i]->getName() == name) {
					return channels[i];
				}
			}			
			return NULL;
		}

		void setChannelByName(const std::string& name, Channel* const channel) {
			
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount ; i++) {
				if(channels[i]->getName() == name) {
					if(channels[i]->getOwner() == this) {
            printf("Replacing an owned channel. Maybe it should be deleted?");
					}
					channels[i] = channel;
					return;
				}
			}
			channels.push_back(channel);
		}

		void setRealization(Channel* const attributeChannel, Channel* const complexChannel) {
			std::vector<Realization::iterator> pending;
			for(	Realization::iterator it = realization.begin();
					it != realization.end();
					++it)
			{
				if(it->first->getName() == attributeChannel->getName() && it->second->getName() == complexChannel->getName()) {
					pending.push_back(it);
				}
				if(it->first->getName() == complexChannel->getName() && it->second->getName() == attributeChannel->getName()) {
					pending.push_back(it);
				}
			}
			
			const int count = int(pending.size());
			for( int i = 0; i < count; i++) {
				realization.erase(pending[i]);
			}

			addRealization(attributeChannel, complexChannel);
		}

		void addRealization(Channel* const attributeChannel, Channel* const complexChannel) {
			realization.insert(std::pair<Channel*, Channel*>(complexChannel, attributeChannel));
			realization.insert(std::pair<Channel*, Channel*>(attributeChannel, complexChannel));
		}

		Channel* getRealization(const Channel* const attributeChannel) {
			const Realization::const_iterator search = realization.find(const_cast<Channel*>(attributeChannel));
			return search == realization.end() ? NULL : search->second;
		}
		
		const Channel* getRealization(const Channel* const attributeChannel) const {
			const Realization::const_iterator search = realization.find(const_cast<Channel*>(attributeChannel));
			return search == realization.end() ? NULL : search->second;
		}

		bool isComplexChannel(const Channel* const channel) const {
#ifdef __CUDACC__
			// Comparison between typeid is not available in the current CUDA compiler
			assert(false);
			return false;
#else
			return 
				typeid(*channel) == typeid(const TriChannel) ||
				typeid(*channel) == typeid(const FlatTriChannel) ||
				channel->getName().find("Tri") != std::string::npos;
#endif
		}

		bool isAttributeChannel(const Channel* const channel) const {
			return !isComplexChannel(channel);
		}

		/*
		vector<Channel*> getAttributeChannelFromComplexChannel(const Channel* const complexChannel) {
		}

		vector<const Channel*> getAttributeChannelFromComplexChannel(const Channel* const complexChannel) const {
		}
		*/

		std::vector<Channel*> getAttributeChannels() {
			std::vector<Channel*> attributeChannels;
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount; i++) {
				Channel* const channel = channels[i];
				if(isAttributeChannel(channel)) {
					attributeChannels.push_back(channel);
				}
			}
			return attributeChannels;
		}

		std::vector<const Channel*> getAttributeChannels() const {
			std::vector<const Channel*> attributeChannels;
			const int channelCount = int(channels.size());
			for(int i = 0; i < channelCount; i++) {
				Channel* const channel = channels[i];
				if(isAttributeChannel(channel)) {
					attributeChannels.push_back(channel);
				}
			}
			return attributeChannels;
		}
    /*
		std::string convertToString() const {
			
			const int channelCount = int(channels.size());			

			int memoryUsage = 0;
			for(int i = 0; i < channelCount; i++) {
				memoryUsage += channels[i]->getMemoryUsage();
			}

			std::string s;

			s += 
				StringTools::intToString(channelCount) + 
				" chanels, " + 
				StringTools::floatToString(float(memoryUsage) / 1000000.0f, 2)  + 
				" MB\n\n";

			for(int i = 0; i < channelCount; i++) {
				const Channel* const channel = channels[i];
				const int channelMemoryUsage = channel->getMemoryUsage();
				s +=
					std::string("'") + 
					channel->getName() + 
					std::string("' ") + 
					StringTools::floatToString(float(channelMemoryUsage) / 1000000.0f, 2) + " MB, " +
					StringTools::intToString(channel->getSize()) + 
					" values\n" +
#ifdef __CUDACC__
					// RTTI with CUDACC is a problem
					"" + 				
#else
					std::string(typeid(*channel).name()) + 
#endif
					((channel->getOwner() == this) ? "" : " (instanced)") + 
					channel->convertToString() + 
					std::string("\n\n");
			}

			s += "\n";

			for(	Realization::const_iterator it = realization.begin();
					it != realization.end();
					++it)
			{
				if(!it->first) {
					s += "NULL";
				} else {
					s += "'" + (it->first)->getName() + "'";
					s += (it->first)->getOwner() == this ? "" : " (instanced)";				
				}
				s += " realizes ";
				if(!it->second) {
					s += "NULL";
				} else {
					s += "'" + (it->second)->getName() + "'";
					s += (it->second)->getOwner() == this ? "" : " (instanced)";
				}
				s += "\n";
			}
			
			return s;
    }
    */
	};

// 	class MeshTypeTrait {
// 
// 	public:
// 
// 		typedef Mesh Type;
// 
// 		static const char* getName() {
// 			return "Mesh";
// 		}
// 	};
// 
// 	typedef Type<MeshTypeTrait> MeshType;
};
