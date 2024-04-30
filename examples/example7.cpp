#include <filesystem>
#include <fstream>
#include <iostream>
#include <miniz/miniz.h>

#if defined(_WIN32)
/* The 'MakeDirectory' function is used to create a directory (folder) in the file system.*/
void MakeDirectory(std::string dir)
{
    try
    {
        std::string currentDir;
        std::string fullPath = "";
        std::string delimiter = "\\";
        size_t pos = 0;
        while ((pos = dir.find(delimiter)) != std::string::npos)
        {
            currentDir = dir.substr(0, pos);
            if (fullPath != "")
            {
                fullPath = fullPath + "\\" + currentDir;
                if (std::filesystem::exists(fullPath) == false)
                {
                    std::filesystem::create_directory(fullPath);
                }
            }
            else
            {
                fullPath = currentDir + "\\";
            }
            dir.erase(0, pos + delimiter.length());
        }
        if (fullPath != "")
        {
            fullPath = fullPath + "\\" + dir;
        }
        else
        {
            fullPath = dir + "\\";
        }
        if (std::filesystem::exists(fullPath) == false)
        {
            std::filesystem::create_directory(fullPath);
        }
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << std::endl;
    }
}
#elif defined(__linux__) || defined(__APPLE__)
/*The `MakeDirectory` function is responsible for creating a directory (folder) in the file system.*/
void MakeDirectory(std::string dir)
{
    std::string currentDir;
    std::string fullPath = "";
    std::string delimiter = "/";
    size_t pos = 0;
    while ((pos = dir.find(delimiter)) != std::string::npos)
    {
        currentDir = dir.substr(0, pos);
        if (fullPath != "")
        {
            fullPath = fullPath + "/" + currentDir;
            if (std::filesystem::exists(fullPath) == false)
            {
                std::filesystem::create_directory(fullPath);
            }
        }
        else
        {
            fullPath = "/" + currentDir;
        }
        dir.erase(0, pos + delimiter.length());
    }
    fullPath = fullPath + "/" + dir;
    if (std::filesystem::exists(fullPath) == false)
    {
        std::filesystem::create_directory(fullPath);
    }
}
#endif

void UnpackArchive(std::string path_from, std::string path_to)
{
    // std::string unpack_command = "tar -xf" + path_from + " --directory " + path_to;
    // system(unpack_command.c_str());
    try
    {
        MakeDirectory(path_to);

        mz_zip_archive zip_archive;
        memset(&zip_archive, 0, sizeof(zip_archive));

        mz_zip_reader_init_file(&zip_archive, path_from.c_str(), 0);

        for (int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
        {
            mz_zip_archive_file_stat file_stat;
            mz_zip_reader_file_stat(&zip_archive, i, &file_stat);

            std::string output_path = path_to + "/" + file_stat.m_filename;
            std::filesystem::path path(output_path);
            std::filesystem::create_directories(path.parent_path());

            std::ofstream out(output_path, std::ios::binary);
            if (!out)
            {
                std::cerr << "Failed to create file: " << output_path << std::endl;
                continue;
            }

            void *fileData = mz_zip_reader_extract_to_heap(&zip_archive, file_stat.m_file_index, &file_stat.m_uncomp_size, 0); // You can adjust the flags parameter as needed
            if (!fileData)
            {
                std::cerr << "Failed to extract file: " << file_stat.m_filename << std::endl;
                continue;
            }

            out.write(static_cast<const char *>(fileData), file_stat.m_uncomp_size);
            mz_free(fileData);

            out.close();
        }

        mz_zip_reader_end(&zip_archive);
    }
    catch (std::exception &error)
    {
        std::cerr << error.what() << std::endl;
    }
}

int main()
{
    std::string zip_file = "archive.zip";
    std::string output_dir = std::filesystem::current_path().generic_string();
    UnpackArchive(zip_file, output_dir);

    return 0;
}