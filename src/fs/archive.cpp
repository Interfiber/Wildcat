//
// Created by hstasonis on 12/31/25.
//

#include <Wildcat/fs/archive.h>

void WildcatArchiveImporter::addArchivePair2(WildcatExternalArchive_Check isValidArchive,
    WildcatExternalArchive* archive)
{
    m_archives.push_back({ isValidArchive, archive });
}

bool WildcatArchiveImporter::importBuffer(const std::string& buffer) const
{
    for (const auto &pair : m_archives)
    {
        if (std::invoke(pair.check, pair.archive,buffer)) // Valid archive found, importArchive can be called
        {
            pair.archive->importArchive(buffer);

            return true;
        }
    }

    return false;
}

std::vector<WildcatArchiveImporter::ArchivePair> WildcatArchiveImporter::getArchivePairs() const
{
    return m_archives;
}
