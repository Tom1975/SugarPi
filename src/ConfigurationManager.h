#pragma once

#include "CPCCore/CPCCoreEmu/simple_string.h"
#include "CPCCore/CPCCoreEmu/IConfiguration.h"
#include "CPCCore/CPCCoreEmu/simple_vector.hpp"

class ConfigurationManager : public IConfiguration
{
public:

   ConfigurationManager();
   virtual ~ConfigurationManager();

   virtual void OpenFile(const char* config_file);
   virtual void CloseFile();

   virtual void SetConfiguration(const char* section, const char* key, const char* value, const char* file);
   virtual void SetConfiguration(const char* section, const char* key, const char* value);

   virtual unsigned int GetConfiguration(const char* section, const char* cle, const char* default_value, char* out_buffer, unsigned int buffer_size, const char* file);
   virtual unsigned int GetConfiguration(const char* section, const char* cle, const char* default_value, char* out_buffer, unsigned int buffer_size);

   virtual unsigned int GetConfigurationInt(const char* section, const char* cle, unsigned int default_value, const char* file);
   virtual unsigned int GetConfigurationInt(const char* section, const char* cle, unsigned int default_value);

   // Section number
   virtual const char* GetFirstSection();
   virtual const char* GetNextSection();

   // Key
   virtual const char* GetFirstKey(const char* section);
   virtual const char* GetNextKey();

protected:
   void Clear();

   template<typename T> 
   class Association
   {
      public:
         std::string key;
         T value;
   };

   class Section : public std::vector <Association<std::string>>
   {
   };

   class ConfigFile : public std::vector <Association<Section>>
   {

   };

   //typedef std::map <std::string, data* > ConfigFile;

   ConfigFile config_file_;
   std::string current_config_file_;

   // Internal Iterator
   std::vector <Association<Section>>::iterator it_section_;
   std::vector <Association<std::string>>::iterator it_key_;
   Section* current_key_section_it_;
};
