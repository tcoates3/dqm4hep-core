/// \file DQMStorage.h
/*
 *
 * DQMStorage.h header template automatically generated by a class generator
 * Creation date : jeu. f�vr. 19 2015
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

#ifndef DQM4HEP_STORAGE_H
#define DQM4HEP_STORAGE_H

// -- dqm4hep headers
#include <dqm4hep/Directory.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    template <typename T>
    class Storage {
    public:
      typedef typename Directory<T>::DirectoryPtr DirectoryPtr;
      typedef typename Directory<T>::ObjectPtr ObjectPtr;
      typedef typename Directory<T>::ObjectList ObjectList;
      typedef typename Directory<T>::DirectoryList DirectoryList;

      Storage();
      ~Storage();
      StatusCode mkdir(const std::string &dirName);
      void cd();
      StatusCode cd(const std::string &dirName);
      bool dirExists(const std::string &dirName) const;
      const std::string &pwd() const;
      StatusCode goUp();
      StatusCode rmdir(const std::string &dirName);
      StatusCode find(const std::string &dirName, DirectoryPtr &directory) const;
      DirectoryPtr root() const;
      DirectoryPtr current() const;
      StatusCode add(ObjectPtr object);
      StatusCode add(const std::string &dirName, ObjectPtr object);
      template <typename F>
      StatusCode remove(F function);
      template <typename F>
      StatusCode remove(const std::string &dirName, F function);
      template <typename F>
      ObjectPtr findObject(F function) const;
      template <typename F>
      ObjectPtr findObject(const std::string &dirName, F function) const;
      bool containsObject(const ObjectPtr &object) const;
      bool containsObject(const std::string &dirName, const ObjectPtr &object) const;
      template <typename F>
      void iterate(F function) const;
      void getObjects(ObjectList &objectList) const;
      void clear();

    private:
      template <typename F>
      bool iterate(const DirectoryPtr &directory, F function) const;

    private:
      DirectoryPtr m_rootDirectory;
      DirectoryPtr m_currentDirectory;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Storage<T>::Storage() : m_rootDirectory(Directory<T>::make_shared("")), m_currentDirectory(m_rootDirectory) {
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Storage<T>::~Storage() = default;

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::mkdir(const std::string &dirName) {
      if (dirName.empty())
        return STATUS_CODE_INVALID_PARAMETER;

      Path path(dirName);

      if (!path.isValid())
        return STATUS_CODE_INVALID_PARAMETER;

      DirectoryPtr directory = !path.isRelative() ? m_rootDirectory : m_currentDirectory;
      StringVector directoryList = path.getSplitPath();

      for (auto iter = directoryList.begin(), endIter = directoryList.end(); endIter != iter; ++iter) {
        std::string dirName = *iter;

        if (dirName == ".")
          continue;

        if (dirName == "..") {
          if (directory == m_rootDirectory)
            return STATUS_CODE_FAILURE;

          directory = directory->parent();
          continue;
        }

        // if sub dir doesn't exists, create it
        if (!directory->hasChild(dirName))
          directory->mkdir(dirName);

        // navigate forward
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, directory->find(dirName, directory));
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Storage<T>::cd() {
      m_currentDirectory = m_rootDirectory;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::cd(const std::string &dirName) {
      // go back to sub dir
      if (dirName.empty()) {
        this->cd();
        return STATUS_CODE_SUCCESS;
      }

      Path path(dirName);

      if (!path.isValid())
        return STATUS_CODE_INVALID_PARAMETER;

      DirectoryPtr directory = !path.isRelative() ? m_rootDirectory : m_currentDirectory;
      StringVector directoryList = path.getSplitPath();

      for (auto iter = directoryList.begin(), endIter = directoryList.end(); endIter != iter; ++iter) {
        std::string dirName = *iter;

        if (dirName == ".")
          continue;

        if (dirName == "..") {
          if (directory == m_rootDirectory)
            return STATUS_CODE_FAILURE;

          directory = directory->parent();
          continue;
        }

        // navigate forward
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, directory->find(dirName, directory));
      }

      if (nullptr == directory)
        return STATUS_CODE_FAILURE;

      m_currentDirectory = directory;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Storage<T>::dirExists(const std::string &dirName) const {
      DirectoryPtr directory = nullptr;
      return (this->find(dirName, directory) == STATUS_CODE_SUCCESS);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const std::string &Storage<T>::pwd() const {
      return m_currentDirectory->name();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::goUp() {
      if (m_currentDirectory->isRoot())
        return STATUS_CODE_UNCHANGED;

      m_currentDirectory = m_currentDirectory->parent();

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::rmdir(const std::string &dirName) {
      if (dirName.empty())
        return STATUS_CODE_NOT_ALLOWED;

      DirectoryPtr directory = nullptr;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->find(dirName, directory));

      if (directory == m_rootDirectory)
        return STATUS_CODE_NOT_ALLOWED;

      std::string fullPathDirName = directory->fullPath().getPath();
      std::string currentFullPathDirName = m_currentDirectory->fullPath().getPath();
      size_t pos = currentFullPathDirName.find(fullPathDirName);

      // this mean that the directory that we try
      // to remove is a parent of the current one.
      if (pos == 0 || pos != std::string::npos)
        return STATUS_CODE_FAILURE;

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, directory->parent()->rmdir(directory->name()));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::find(const std::string &dirName, DirectoryPtr &directory) const {
      directory = nullptr;

      // go back to sub dir
      if (dirName.empty() || dirName == "." || dirName == "./") {
        directory = m_currentDirectory;
        return STATUS_CODE_SUCCESS;
      }

      Path path(dirName);

      if (!path.isValid())
        return STATUS_CODE_INVALID_PARAMETER;

      directory = !path.isRelative() ? m_rootDirectory : m_currentDirectory;
      StringVector directoryList = path.getSplitPath();

      for (auto iter = directoryList.begin(), endIter = directoryList.end(); endIter != iter; ++iter) {
        std::string dirName = *iter;

        if (dirName == ".")
          continue;

        if (dirName == "..") {
          if (directory == m_rootDirectory)
            return STATUS_CODE_FAILURE;

          directory = directory->parent();
          continue;
        }

        // navigate forward
        StatusCode statusCode = directory->find(dirName, directory);

        if (statusCode != STATUS_CODE_SUCCESS)
          return statusCode;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Storage<T>::DirectoryPtr Storage<T>::root() const {
      return m_rootDirectory;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Storage<T>::DirectoryPtr Storage<T>::current() const {
      return m_currentDirectory;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::add(ObjectPtr object) {
      return m_currentDirectory->add(object);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Storage<T>::add(const std::string &dirName, ObjectPtr object) {
      DirectoryPtr directory = nullptr;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->mkdir(dirName));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->find(dirName, directory));
      return directory->add(object);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline StatusCode Storage<T>::remove(F function) {
      return m_currentDirectory->remove(function);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline StatusCode Storage<T>::remove(const std::string &dirName, F function) {
      DirectoryPtr directory = nullptr;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->find(dirName, directory));
      return directory->remove(function);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline typename Storage<T>::ObjectPtr Storage<T>::findObject(F function) const {
      return m_currentDirectory->find(function);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline typename Storage<T>::ObjectPtr Storage<T>::findObject(const std::string &dirName, F function) const {
      DirectoryPtr directory;

      if (this->find(dirName, directory))
        return nullptr;

      return directory->find(function);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Storage<T>::containsObject(const ObjectPtr &object) const {
      return m_currentDirectory->containsObject(object);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Storage<T>::containsObject(const std::string &dirName, const ObjectPtr &object) const {
      DirectoryPtr directory = NULL;

      if (this->find(dirName, directory))
        return false;

      return directory->containsObject(object);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline bool Storage<T>::iterate(const DirectoryPtr &directory, F function) const {
      auto contents(directory->contents());

      for (auto &obj : contents)
        if (!function(directory, obj))
          return false;

      auto subdirs(directory->subdirs());

      for (const auto &dir : subdirs)
        if (!this->iterate(dir, function))
          return false;

      return true;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline void Storage<T>::iterate(F function) const {
      this->iterate(m_rootDirectory, function);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Storage<T>::getObjects(ObjectList &objectList) const {
      this->iterate([&](const DirectoryPtr &directory, ObjectPtr object) {
        objectList.push_back(object);
        return true;
      });
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Storage<T>::clear() {
      m_rootDirectory->clear();
      m_currentDirectory = m_rootDirectory;
    }
  }
}

#endif //  DQM4HEP_STORAGE_H
