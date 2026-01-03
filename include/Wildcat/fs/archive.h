//
// Created by hstasonis on 12/31/25
// 

#pragma once
#include <string>
#include <sstream>
#include <vector>

typedef bool (*WildcatExternalArchive_Check)(const std::string&);

inline std::vector<std::string> Helper_Split(const std::string &s, const char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


/**
 * External archive format which can be imported into wildcat
 */
class WildcatExternalArchive
{
public:
    WildcatExternalArchive() = default;
    virtual ~WildcatExternalArchive() = default;

    /**
     * Import this archive into Wildcat
     * @param buffer Input archive buffer
     */
    virtual void importArchive(const std::string &buffer) = 0;


    /**
     * Return the archive name for use in user interfaces
     */
    [[nodiscard]] virtual std::string getArchiveName() = 0;
};

class WildcatArchiveImporter
{
public:
    WildcatArchiveImporter() = default;

    [[nodiscard]] static WildcatArchiveImporter* get()
    {
        static auto importer = new WildcatArchiveImporter();

        return importer;
    }

    struct ArchivePair
    {
        /// @brief  Check if the input string is a valid buffer of this type, if true the `archive` is imported
        WildcatExternalArchive_Check check;

        WildcatExternalArchive* archive;
    };

    /**
     * Add a new archive pair to the importer
     * @param isValidArchive Function to check if an input string is this type of archive
     * @param archive External archive class
     */
    void addArchivePair(WildcatExternalArchive_Check isValidArchive, WildcatExternalArchive* archive);

    /**
     * Import `buffer` into Wildcat using a valid archive pair
     * @param buffer Buffer to import
     * @return true if a valid archive pair was found, false if otherwise
     */
    [[nodiscard]] bool importBuffer(const std::string &buffer) const;

    /**
     * Return the list of registered archive pairs
     */
    [[nodiscard]] std::vector<ArchivePair> getArchivePairs() const;

private:
    std::vector<ArchivePair> m_archives{};
};

/**
 * Paste from google sheets, excel, etc
 */
class WildcatPastedSheetArchive : public WildcatExternalArchive
{
public:
    WildcatPastedSheetArchive() = default;

    void importArchive(const std::string& buffer) override;
    [[nodiscard]] std::string getArchiveName() override;

    static bool isValid(const std::string &buffer);
};

class WildcatCSVArchive : public WildcatExternalArchive
{
public:
    WildcatCSVArchive() = default;

    void importArchive(const std::string& buffer) override;
    [[nodiscard]] std::string getArchiveName() override;

    static bool isValid(const std::string &buffer);
};
