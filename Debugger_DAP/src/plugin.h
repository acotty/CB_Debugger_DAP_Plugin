/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef _DEBUGGER_DAP_PLUGIN_H_
#define _DEBUGGER_DAP_PLUGIN_H_

// System and library includes
#include <tinyxml2.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    // For compilers that support precompilation, includes <wx/wx.h>
    #include <wx/wx.h>
#endif
#include <memory>

// CB includes
#include <cbplugin.h> // for "class cbPlugin"

// Debugger includes
#include "debugger_logger.h"
#include "definitions.h"

// DAP includes
#include "Client.hpp"
#include "Process.hpp"

class TextCtrlLogger;
class Compiler;

namespace dbg_DAP
{
    class DebuggerConfiguration;

} // namespace dbg_DAP

class Debugger_DAP : public cbDebuggerPlugin
{
    public:
        /** Constructor. */
        Debugger_DAP();
        /** Destructor. */
        virtual ~Debugger_DAP();

    public:
        virtual void SetupToolsMenu(wxMenu & menu);
        virtual bool ToolMenuEnabled() const
        {
            return true;
        }

        virtual bool SupportsFeature(cbDebuggerFeature::Flags flag);
        virtual cbDebuggerConfiguration * LoadConfig(const ConfigManagerWrapper & config);
        dbg_DAP::DebuggerConfiguration & GetActiveConfigEx();
        cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);
        virtual bool Debug(bool breakOnEntry);
        virtual void Continue();
        virtual bool RunToCursor(const wxString & filename, int line, const wxString & line_text);
        virtual void SetNextStatement(const wxString & filename, int line);
        virtual void Next();
        virtual void NextInstruction();
        virtual void StepIntoInstruction();
        virtual void Step();
        virtual void StepOut();
        virtual void Break();
        virtual void Stop();
        virtual bool IsRunning() const;
        virtual bool IsStopped() const;
        virtual bool IsBusy() const;
        virtual int GetExitCode() const;
        void SetExitCode(int code)
        {
            m_exit_code = code;
        }

        // stack frame calls;
        virtual int GetStackFrameCount() const;
        virtual cb::shared_ptr<const cbStackFrame> GetStackFrame(int index) const;
        virtual void SwitchToFrame(int number);
        virtual int GetActiveStackFrame() const;

        // breakpoints calls
        virtual cb::shared_ptr<cbBreakpoint> AddBreakpoint(const wxString & filename, int line);
        cb::shared_ptr<cbBreakpoint> UpdateOrAddBreakpoint(const wxString & filename, const int line, const int id);
//        cb::shared_ptr<cbBreakpoint> AddBreakpoint(cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp);
        virtual cb::shared_ptr<cbBreakpoint> AddDataBreakpoint(const wxString & dataExpression)  {return 0;};
        virtual int GetBreakpointsCount() const;
        virtual cb::shared_ptr<cbBreakpoint> GetBreakpoint(int index);
        virtual cb::shared_ptr<const cbBreakpoint> GetBreakpoint(int index) const;
        cb::shared_ptr<cbBreakpoint> GetBreakpointByID(int id);
        virtual void UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) {};
        virtual void DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) {};
        virtual void DeleteAllBreakpoints() {};
        virtual void ShiftBreakpoint(int index, int lines_to_shift) {};
        virtual void EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable) {};

        // threads
        virtual int GetThreadsCount() const {return 0;};
        virtual cb::shared_ptr<const cbThread> GetThread(int index) const  {return nullptr;};
        virtual bool SwitchToThread(int thread_number) { return false;};

        // watches
        void UpdateDAPWatches(int updateType);
        virtual cb::shared_ptr<cbWatch> AddWatch(const wxString & symbol, bool update);
        virtual cb::shared_ptr<cbWatch> AddWatch(dbg_DAP::GDBWatch * watch, cb_unused bool update);
        cb::shared_ptr<cbWatch> AddMemoryRange(uint64_t address, uint64_t size, const wxString &symbol, bool update)  {return 0;};
//        void AddTooltipWatch(const wxString & symbol, wxRect const & rect);
        virtual void DeleteWatch(cb::shared_ptr<cbWatch> watch);
        virtual bool HasWatch(cb::shared_ptr<cbWatch> watch);
        bool IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> &watch);
        virtual void ShowWatchProperties(cb::shared_ptr<cbWatch> watch);
        virtual bool SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value);
        virtual void ExpandWatch(cb::shared_ptr<cbWatch> watch);
        virtual void CollapseWatch(cb::shared_ptr<cbWatch> watch);
        virtual void UpdateWatch(cb::shared_ptr<cbWatch> watch);

        //
        virtual void SendCommand(const wxString & cmd, bool debugLog) {};
        virtual void AttachToProcess(const wxString & pid) {};
        virtual void DetachFromProcess() {};
        virtual bool IsAttachedToProcess() const {return 0;};
        virtual void GetCurrentPosition(wxString & filename, int & line) {};

        virtual void RequestUpdate(DebugWindows window);
//        virtual void OnValueTooltip(const wxString & token, const wxRect & evalRect);
//        virtual bool ShowValueTooltip(int style);

        void StripQuotes(wxString& str);
        void ConvertToGDBFriendly(wxString& str);
        void ConvertToGDBDirectory(wxString& str, wxString base = "", bool relative = true);
        wxArrayString ParseSearchDirs(cbProject *project);
//        TiXmlElement* GetElementForSaving(cbProject &project, const char *elementsToClear);
//        void SetSearchDirs(cbProject &project, const wxArrayString &dirs);

//        dbg_DAP::RemoteDebuggingMap ParseRemoteDebuggingMap(cbProject &project);
//        void SetRemoteDebuggingMap(cbProject &project, const dbg_DAP::RemoteDebuggingMap &map);

    protected:
        /** Any descendent plugin should override this virtual method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        virtual void OnAttachReal();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnReleaseReal(bool appShutDown);

    protected:
        virtual void ConvertDirectory(wxString & /*str*/, wxString /*base*/, bool /*relative*/);
        virtual cbProject * GetProject()
        {
            return m_pProject;
        }
        virtual void ResetProject()
        {
            m_pProject = NULL;
        }
        void OnProjectOpened(CodeBlocksEvent& event);
        virtual void CleanupWhenProjectClosed(cbProject * project);
        virtual bool CompilerFinished(bool compilerFailed, StartType startType);

    public:
        void UpdateWhenStopped();
        void UpdateOnFrameChanged(bool wait);
//        dbg_DAP::GDBCurrentFrame & GetGDBCurrentFrame()
//        {
//            return m_current_frame;
//        }
//
//        dbg_DAP::GDBExecutor & GetGDBExecutor()
//        {
//            return m_executor;
//        }

        dbg_DAP::LogPaneLogger * GetGDBLogger()
        {
            return m_pLogger;
        }
    private:
        DECLARE_EVENT_TABLE();

        void OnTimer(wxTimerEvent & event);
        void OnIdle(wxIdleEvent & event);
        void OnMenuInfoCommandStream(wxCommandEvent & event);
        wxString GetShellString();
        void LaunchDAPDebugger(const wxString & dap_debugger, const wxString & dap_port_number);
        int LaunchDebugger(cbProject * project,
                           const wxString & dap_debugger,
                           const wxString & debuggee,
                           const wxString & dap_port_number,
                           const wxString & working_dir,
                           int pid,
                           bool console,
                           StartType start_type);
        bool SelectCompiler(cbProject & project, Compiler *& compiler, ProjectBuildTarget *& target, long pid_to_attach);
        int StartDebugger(cbProject * project, StartType startType);
        void CreateStartBreakpoints(bool force);
        void CreateStartWatches();
        bool SaveStateToFile(cbProject* prj);
        bool LoadStateFromFile(cbProject* prj);
        void DoWatches();

    private:
        wxTimer m_timer_poll_debugger;
        cbProject * m_pProject;

//        dbg_DAP::GDBExecutor m_executor;
//        dbg_DAP::ActionsMap  m_actions;
        dbg_DAP::LogPaneLogger * m_pLogger;
//        dbg_DAP::GDBThreadsContainer m_threads;
//
//
//        dbg_DAP::GDBMemoryRangeWatchesContainer m_memoryRanges;
//        dbg_DAP::GDBTextInfoWindow * m_command_stream_dialog;
        int m_exit_code;
//        int m_console_pid;
//        int m_pid_attached;
        bool m_hasStartUpError;


    private:
        dap::Client m_dapClient;
        long m_dapPid;
        wxString m_dap_debuggee;
        wxString m_dap_debuggeepath;
        dap::Process* m_process = nullptr;

        enum DAPState
        {
            NotConnected = 0,
            Connected,
            Stopped,
            Running
        };
        DAPState  DAPDebuggerState;

        // breakpoints
        dbg_DAP::GDBBreakpointsContainer m_breakpoints;
        std::map<wxString, std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>>> m_map_filebreakpoints;
        dbg_DAP::GDBBreakpointsContainer m_temporary_breakpoints;

        // Stack
        dbg_DAP::GDBCurrentFrame m_current_frame;
        dbg_DAP::GDBBacktraceContainer m_backtrace;

        // Watches
        dbg_DAP::GDBWatchesContainer m_watches;
        dbg_DAP::GDBMapWatchesToType m_mapWatchesToType;
        cb::shared_ptr<dbg_DAP::GDBWatch> m_WatchLocalsandArgs;
        std::vector<dap::Variable> m_stackdapvariables;

        // misc
        void KillDAPDebugger();
        void OnProcessBreakpointData(const wxString& brkDescription);

        /// Dap events
        void OnStopped(DAPEvent& event);
        void OnStackTrace(DAPEvent& event);
        void OnScopes(DAPEvent& event);
        void OnVariables(DAPEvent& event);
        void OnInitializedEvent(DAPEvent& event);
        void OnInitializeResponse(DAPEvent& event);
        void OnExited(DAPEvent& event);
        void OnTerminated(DAPEvent& event);
        void OnOutput(DAPEvent& event);
        void OnBreakpointLocations(DAPEvent& event);
        void OnConnectionError(DAPEvent& event);
        void OnBreakpointSet(DAPEvent& event);
        void OnLaunchResponse(DAPEvent& event);
        void OnRunInTerminalRequest(DAPEvent& event);
        void OnDapLog(DAPEvent& event);
};

#endif // _DEBUGGER_DAP_PLUGIN_H_
