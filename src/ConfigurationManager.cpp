#include "ConfigurationManager.h"

#include <SDCard/emmc.h>
#include <fatfs/ff.h>

 CLogger* log_s = nullptr;

/////////////////////////////////////////////////////////////
/// Helper functions
ConfigurationManager::ConfigurationManager(CLogger* log) : logger_(log)
{
   log_s = log;
}

ConfigurationManager::~ConfigurationManager()
{
   // Clear everything
   Clear();
}

void ConfigurationManager::Clear()
{
/*   for (auto const& ent1 : config_file_)
   {
      // ent1.first is the first key
      for (auto const& ent2 : ent1)
      {
         // ent2.first is the second key
         // ent2.second is the data
      }
   }*/
   config_file_.clear();
}

unsigned int ConfigurationManager::getline ( const char* buffer, int size, std::string& out)
{
   if ( size == 0)
   {
      return 0;
   }
      
   // looking for /n
   int offset = 0;
   while (buffer[offset] != 0x0A && buffer[offset] != 0x0D && offset < size)
   {
      offset++;
   }

   char* line = new char[offset+1];
   memcpy ( line, buffer, offset);
   line[offset] = '\0';
   out = std::string(line);
   delete []line;
   return (offset == size)?offset:offset+1;
}

void ConfigurationManager::OpenFile(const char* config_file)
{
   logger_->Write("ConfigurationManager", LogNotice, "OpenFile : %s", config_file);
   if (current_config_file_.compare( config_file) == 0)
   {
      // already openend
      return;
   }
   current_config_file_ = config_file;
   Clear();
   std::string s, key, value;
   std::string current_section = "";

   FIL File;
   FRESULT Result = f_open(&File, config_file, FA_READ | FA_OPEN_EXISTING);   
   if (Result != FR_OK)
   {
      logger_->Write("ConfigurationManager", LogNotice, "Cannot open file: %s", config_file);
      return;
   }

   FILINFO file_info;
   f_stat(config_file, &file_info);
   unsigned char* buff = new unsigned char[file_info.fsize];
   unsigned nBytesRead;

   f_read(&File, buff, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      // ERROR
      f_close(&File);
      logger_->Write("ConfigurationManager", LogNotice, "Read incorrect %i instead of ", nBytesRead, file_info.fsize);
      return;
   }

   logger_->Write("ConfigurationManager", LogNotice, "Read Config : %s  ",buff);

   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   while ((end_line = getline(&ptr_buffer[offset], nBytesRead, s)) > 0)
   {
      offset += end_line;
      nBytesRead -= end_line;

      // Skip empty lines
      if (s.size() == 0) continue;

      std::string::size_type begin = s.find_first_not_of(" \f\t\v");
      // Skip blank lines
      if (begin == std::string::npos) continue;

      // Skip commentary
      if (  s[begin] == '#' 
         || s[begin] == ';' ) continue;

      
      std::string::size_type begin_section = s.find('[', begin);      
      //if (begin_section != std::string::npos)
      if ( s[begin] == '[')
      {
         begin_section = begin;
         std::string::size_type end_section = s.find(']');
         if (end_section != std::string::npos)
         {
            current_section = s.substr(begin_section+1, end_section - 1);
            s = s.substr(end_section+1);

            logger_->Write("ConfigurationManager", LogNotice, "READ section %s", current_section.c_str());
         }
      }

      // Search key (if a section is already defined)
      if (current_section.size() > 0)
      {
         // Extract the key value
         std::string::size_type end = s.find('=', begin);

         if (end == std::string::npos) continue;

         logger_->Write("ConfigurationManager", LogNotice, "READ key/value %s", s.c_str());
         key = s.substr(begin, end - begin);
         // (No leading or trailing whitespace allowed)
         size_t last_of_space = key.find_last_not_of(" \f\t\v") ;
         if ( last_of_space != std::string::npos)
            key.erase(last_of_space+ 1);        

         // No blank keys allowed
         if (key.size() == 0) continue;

         // Extract the value (no leading or trailing whitespace allowed)
         begin = s.find_first_not_of(" \f\n\r\t\v", end + 1);
         if (begin == std::string::npos)
         {
            value = "";
         }
         else
         {
            // Remove ending spaces
            end = s.find_last_not_of(" \f\n\r\t\v");
            if ( end == std::string::npos)
            {
               logger_->Write("ConfigurationManager", LogNotice, "end == std::string::npos");
               value = s.substr(begin);
            }
            else
            {
               logger_->Write("ConfigurationManager", LogNotice, "end != std::string::npos");
               value = s.substr(begin, end-begin);
            }

            logger_->Write("ConfigurationManager", LogNotice, "READ key : %s", key.c_str());
            logger_->Write("ConfigurationManager", LogNotice, "READ value: %s ", value.c_str());

            // Add this key/value to current section
            Section* section = nullptr;
            if (config_file_.GetSection (current_section.c_str(), section) == false)
            {
               Association<Section*> new_section;
               new_section.key = current_section;
               section = new_section.value = new Section();
               config_file_.push_back(new_section);
            }
            Association<std::string> new_assoc;
            new_assoc.key = key;
            new_assoc.value = value;
            section->push_back(new_assoc);
         }
      }
   }
   delete []buff;
   f_close(&File);
}

void ConfigurationManager::CloseFile()
{
   FIL File;
   FRESULT Result = f_open(&File, current_config_file_.c_str(), FA_WRITE | FA_CREATE_ALWAYS );   
   if (Result != FR_OK)
   {
      logger_->Write("ConfigurationManager", LogNotice, "Cannot open file: %s", current_config_file_.c_str());
      return;
   }

   logger_->Write("ConfigurationManager", LogNotice, "File open");

   // Write this file
   std::string output_file;
   for (auto const& ent1 : config_file_)
   {
      output_file.append("[");
      output_file.append(ent1.key);
      output_file.append("]\r\n");
      logger_->Write("ConfigurationManager", LogNotice, "section %s", ent1.key.c_str());
      for (auto const& ent2 : *ent1.value)
      {
         // ent2.first is the second key
         output_file.append (ent2.key);
         logger_->Write("ConfigurationManager", LogNotice, "key %s", ent2.key.c_str());
         output_file.append ("=");
         output_file.append (ent2.value);
         logger_->Write("ConfigurationManager", LogNotice, "value %s", ent2.value.c_str());
         output_file.append ("\r\n");
      }
   }
   logger_->Write("ConfigurationManager", LogNotice, "Output file : %s", output_file.c_str());
   unsigned nBytesRead;
   f_write (&File, output_file.c_str(), output_file.size(), &nBytesRead);
   f_close(&File);
}

void ConfigurationManager::SetConfiguration(const char* section, const char* key, const char* value, const char* file)
{
   OpenFile(file);
   SetConfiguration(section, key, value);
}

void ConfigurationManager::SetConfiguration(const char* section_key, const char* key, const char* value)
{
   logger_->Write("ConfigurationManager", LogNotice, "SetConfiguration : [%s] %s=%s", section_key, key ,value);

   Section* section = nullptr;
   if (config_file_.GetSection (section_key, section) == false)
   {
      logger_->Write("ConfigurationManager", LogNotice, "Section not found");
      Association<Section*> new_section;
      new_section.key = section_key;
      section = new_section.value = new Section();
      config_file_.push_back(new_section);
   }

   bool found = false;
   for (auto &it: *section)
   {
      if (strcmp ( it.key.c_str(), key) == 0)
      {
         it.value = value;
         found = true;
      }
   }

   if (!found)
   {
      logger_->Write("ConfigurationManager", LogNotice, "Key not found");
      Association<std::string> new_assoc;
      new_assoc.key = key;
      new_assoc.value = value;
      section->push_back(new_assoc);
   }

   char output_log_buffer[255];
   GetConfiguration(section_key, key, "NOTHING", output_log_buffer, 255 );

   for (auto const& ent1 : config_file_)
   {
      logger_->Write("ConfigurationManager", LogNotice, "SECTION : %s", ent1.key.c_str());
      for (auto const& ent2 : *ent1.value)
      {
         logger_->Write("ConfigurationManager", LogNotice, "KEYS: %s = VALUE : %s", ent2.key.c_str(), ent2.value.c_str());
      }
   }
   logger_->Write("ConfigurationManager", LogNotice, "EOF");
}

unsigned int ConfigurationManager::GetConfiguration(const char* section, const char* key, const char* default_value, char* out_buffer, unsigned int buffer_size, const char* file)
{
   OpenFile(file);
   return GetConfiguration(section, key, default_value, out_buffer, buffer_size);
   
}

unsigned int ConfigurationManager::GetConfiguration(const char* section_key, const char* key, const char* default_value, char* out_buffer, unsigned int buffer_size)
{
   Section* section = nullptr;
   if (config_file_.GetSection (section_key, section))
   {
      std::string value_str;
      if ( section->GetKey(key, &value_str))
      {
         strncpy ( out_buffer, value_str.c_str(), buffer_size);
         return strlen(out_buffer);
      }
   }
   strncpy(out_buffer, default_value, buffer_size);
   return 0;
}

unsigned int ConfigurationManager::GetConfigurationInt(const char* section_key, const char* key, unsigned int default_value, const char* file)
{
   OpenFile(file);
   return GetConfigurationInt(section_key, key, default_value);
}

unsigned int ConfigurationManager::GetConfigurationInt(const char* section_key, const char* key, unsigned int default_value)
{
   Section* section = nullptr;
   if (config_file_.GetSection (section_key, section))
   {
      std::string value_str;
      if ( section->GetKey(key, &value_str))
      {
         return atoi(value_str.c_str());
      }
   }
   return default_value;
}

const char* ConfigurationManager::GetFirstSection()
{
   it_section_ = config_file_.begin();
   return GetNextSection();
}

const char* ConfigurationManager::GetNextSection()
{
   if (it_section_ != config_file_.end())
   {
      const char* value = it_section_->key.c_str();
      ++it_section_;
      return value;
   }
   return nullptr;
}

const char* ConfigurationManager::GetFirstKey(const char* section_key)
{
   Section* section = nullptr;
   if (config_file_.GetSection (section_key, section))
   {
      it_key_ = section->begin();
      current_key_section_it_ = section;
      return GetNextKey();
   }
   return nullptr;
}

const char* ConfigurationManager::GetNextKey()
{
   if (it_key_ != current_key_section_it_->end())
   {
      const char* value = it_key_->key.c_str();
      ++it_key_;
      return value;
   }
   return nullptr;
}

bool ConfigurationManager::ConfigFile::GetSection (const char* section_name, ConfigurationManager::Section*& section)
{
   // Look at section
   for (auto &it:*this)
   {
      if (strcmp ( it.key.c_str(), section_name) == 0)
      {
         section = it.value;
         return true;
      }
   }
   return false;
}


bool ConfigurationManager::Section::GetKey (const char* key_name, std::string* key)
{
   // Look at section
   for (auto it:*this)
   {
      if (strcmp ( it.key.c_str(), key_name) == 0)
      {
         *key = it.value;
         return true;
      }
   }
   return false;
}


