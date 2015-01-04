/*
Filename: Platform_win.cpp
Purpose:  Platform specfic interfaces

Part of Engine2D

Copyright (C) 2014 Vbitz

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "Platform.hpp"

#include "Logger.hpp"

#include <iostream>
#include <cstdio>

#include <fcntl.h>

#include <Windows.h>
#include <lmcons.h>

#include <Rpc.h>

namespace Engine {
	namespace Platform {

		ENGINE_CLASS(WindowsLibrary);

		class WindowsLibrary : public Libary {
		public:
			WindowsLibrary() : thisModule(true) {
				this->modulePointer = ::GetModuleHandle(NULL);
			}

			WindowsLibrary(std::string modulePath) {
				this->modulePointer = ::LoadLibraryEx(modulePath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
				if (this->modulePointer == NULL) {
					Logger::begin("WindowsLibrary", Logger::LogLevel_Error) << "Error loading: " << modulePointer << " : " << GetLastError() << Logger::end();
				}
			}

			~WindowsLibrary() override {
				if (!thisModule) FreeLibrary(this->modulePointer); // GetModuleHandle is not reference counted so this will have bad results
				this->modulePointer = NULL;
			}

			bool IsValid() override {
				return this->modulePointer != NULL;
			}

			ENGINE_FARPROC GetMethod(std::string name) override {
				return (ENGINE_FARPROC)GetProcAddress(this->modulePointer, name.c_str());
			}

			bool CallMethod(std::string name) override {
				ENGINE_FARPROC m = this->GetMethod(name);
				if (m != NULL) {
					m();
					return 0;
				}
				else {
					return 1;
				}
			}

			std::vector<std::string> GetExportedMethods() override {
				std::vector<std::string> ret;

				// TODO: open the module file using Filesystem and load the content
				//          this is going to be a real pain

				return ret;
			}

		private:
			HMODULE modulePointer;
			bool thisModule = false;
		};

		ENGINE_CLASS(WindowsThread);

		class WindowsThread : public Thread {
		public:
			WindowsThread(ThreadMethod entry, void* args) {
				if ((this->_thread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) entry, args, 0, NULL)) == NULL) {
					std::cout << "ERROR: CreateThread failed : " << ::GetLastError() << std::endl;
				}

				this->_uuid = GenerateUUID();
			}

			WindowsThread(HANDLE thread) {
				_thread = thread;
			}

			void Terminate() override {

			}

			void Exit(void* ret) override {
				ExitThread((DWORD) ret);
			}

			UUID GetThreadID() override {
				return this->_uuid;
			}

		private:
			HANDLE _thread;
			UUID _uuid;
		};

		ENGINE_CLASS(WindowsMutex);

		class WindowsMutex : public Mutex {
		public:
			WindowsMutex() {
				this->_mutex = ::CreateMutex(NULL, false, NULL);
			}

			~WindowsMutex() override {
				CloseHandle(this->_mutex);
			}

			bool SafeEnter() override {
				if (!this->_entered) {
					this->Enter();
					return true;
				}
				else {
					return false;
				}
			}

			void Enter() override {
				if (::WaitForSingleObject(this->_mutex, 0) == WAIT_OBJECT_0) {
					this->_entered = true;
				}
			}

			void Exit() override {
				ReleaseMutex(this->_mutex);
				this->_entered = false;
			}
		private:
			HANDLE _mutex;
			bool _entered = false;
		};

		class WindowsMemoryMappedFile : public MemoryMappedFile {
		public:
			WindowsMemoryMappedFile(HANDLE fd, FileMode permissions) : _fd(fd), _permissions(permissions){}

			MemoryMappedRegionPtr MapRegion(unsigned long offset, size_t size) override {
				assert(offset % 4096 == 0);
				HANDLE mapping = CreateFileMapping(this->_fd, NULL, _permissions == FileMode::Write ? PAGE_READWRITE : PAGE_READONLY, 0, 0, NULL);
				void *region = NULL;
				return new MemoryMappedRegion(this, region, size);
			}

			void UnmapRegion(MemoryMappedRegionPtr r) override {
				//munmap(r->_data, r->_length);
				delete r;
				this->_mappedRegions--;
			}

			bool IsValid() override {
				DWORD outVar;
				return GetHandleInformation(this->_fd, &outVar);
			}

			void Close() override {
				assert(this->_mappedRegions == 0);
				CloseHandle(this->_fd);
			}

		private:
			HANDLE _fd;
			FileMode _permissions;
			size_t _mappedRegions = 0;
		};

		int _argc;
		const char** _argv;

		void SetRawCommandLine(int argc, const char** argv) {
			_argc = argc;
			_argv = argv;
		}

		int GetRawCommandLineArgC() {
			return _argc;
		}

		const char** GetRawCommandLineArgV() {
			return _argv;
		}

		std::string GetExecutablePath() {
			return "";
		}

		engine_memory_info GetMemoryInfo() {
			engine_memory_info ret;

			return ret;
		}

		double GetMemoryUsage() {
			engine_memory_info mem = GetMemoryInfo();
			return (double)mem.myPhysicalUsed / (double)mem.totalPhysical;
		}

		int GetProcesserCount() {
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			return sysinfo.dwNumberOfProcessors;
		}

		LibaryPtr OpenLibary(std::string path) {
			return new WindowsLibrary(path);
		}

		LibaryPtr GetThisLibary() {
			return new WindowsLibrary();
		}

		ThreadPtr CreateThread(ThreadMethod entry, void* threadArgs) {
			return new WindowsThread(entry, threadArgs);
		}

		ThreadPtr GetCurrentThread() {
			return new WindowsThread(::GetCurrentThread());
		}

		bool IsMainThread() {
			static int mainThreadID = 0;
			if (mainThreadID == 0) {
				mainThreadID = ::GetCurrentThreadId();
			}
			return mainThreadID == ::GetCurrentThreadId();
		}

#ifdef CreateMutex
		// Only Needed on win32
#undef CreateMutex
#endif

		MutexPtr CreateMutex() {
			return new WindowsMutex();
		}

		MemoryMappedFilePtr OpenMemoryMappedFile(std::string filename, FileMode mode) {
			int fmode;
			switch (mode) {
			case FileMode::Read:    fmode = GENERIC_READ; break;
			case FileMode::Write:   fmode = GENERIC_READ | GENERIC_WRITE; break;
			}
			HANDLE fd = CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			assert(fd != NULL);
			return new WindowsMemoryMappedFile(fd, mode);
		}

		static_assert(sizeof(GUID) == sizeof(UUID), "Engine::GUID must match size with UUID (Created with CoCreateGUID");

		UUID GenerateUUID() {
			GUID guid;
			UuidCreate(&guid);
			UUID ret;
			// same structure (rougly)
			memcpy(&ret, &guid, sizeof(UUID));
			return ret;
		}

		std::string StringifyUUID(UUID uuidArr) {
			RPC_CSTR uuidString = NULL;
			::UuidToString((GUID*) &uuidArr, &uuidString);
			std::string ret = std::string((char*) uuidString);
			::RpcStringFree(&uuidString);
			return ret;
		}

		UUID ParseUUID(std::string uuidStr) {
			GUID guid;
			::UuidFromString((RPC_CSTR)uuidStr.c_str(), &guid);
			UUID ret;
			// same structure (rougly)
			memcpy(&ret, &guid, sizeof(UUID));
			return ret;
		}

		double GetTime() {
			static double _startTime = -1;
			static double _freq = -1;

			if (_freq == -1 && _startTime == -1) {
				LARGE_INTEGER freqL, startTimeL;
				::QueryPerformanceFrequency(&freqL);
				_freq = (double)freqL.QuadPart;
				::QueryPerformanceCounter(&startTimeL);
				_startTime = (double)(startTimeL.QuadPart) / _freq;
			}

			LARGE_INTEGER timeL;

			::QueryPerformanceCounter(&timeL);

			double time = (double)(timeL.QuadPart) / _freq;

			return time - _startTime;
		}

		bool ShowMessageBox(std::string title, std::string text, bool modal) {
			MessageBox(NULL, title.c_str(), text.c_str(), NULL);
			return true;
		}

		std::string GetUsername() {
			static TCHAR usernameBuff[UNLEN + 1];
			DWORD usernameLength = UNLEN + 1;
			if (!::GetUserName(usernameBuff, &usernameLength)) {
				Logger::begin("Platform", Logger::LogLevel_Error) << "GetUserName failed with: " << ::GetLastError() << Logger::end();
			}
			std::cout << usernameBuff << " : " << usernameLength << std::endl;
			return std::string((char*) usernameBuff, usernameLength);
		}

		void DumpStackTrace() {
		}

#ifdef ShellExecute
		// Only Needed on win32
#undef ShellExecute
#endif

		int ShellExecute(std::string path) {
			return (int) ::ShellExecuteW(NULL, NULL, (LPCWSTR) path.c_str(), NULL, NULL, NULL) > 32;
		}

		void Sleep(int timeS) {
			::Sleep(timeS);
		}

		void NanoSleep(int timeNS) {
			Sleep(timeNS / 1.0e-9);
		}

		long CryptBytes(unsigned char* buffer, long count) {
			static HCRYPTPROV hCryptProv = NULL;

			if (hCryptProv == NULL) {
				CryptAcquireContext(
					&hCryptProv,               // handle to the CSP
					NULL,                  // container name 
					NULL,                      // use the default provider
					PROV_RSA_FULL,             // provider type
					0);
			}

			if (hCryptProv == NULL) {
				return 0;
			}

			if (CryptGenRandom(hCryptProv, count, buffer)) {
				return count;
			} else {
				return 0;
			}
		}
	}
}
