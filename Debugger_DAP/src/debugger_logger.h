/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef _DEBUGGER_DAP_GDB_LOGGER_H_
#define _DEBUGGER_DAP_GDB_LOGGER_H_

#include <cbplugin.h>
#include <logmanager.h>

class Debugger_DAP;

namespace dbg_DAP
{

    class LogPaneLogger
    {
        public:
            enum LineType
            {
                UserDisplay = 0,
                Info,
                Debug,
                Warning,
                Error,

                GDB_Stop_Start,
                Queue,
                Command,
                CommandResult,
                ProgramState,
                Event,
                Transmit,
                Receive,
                Receive_NoLine,
                Receive_Info
            };

        public:
            LogPaneLogger(Debugger_DAP * dbgGDB);
            ~LogPaneLogger();

            static void LogGDBMsgType(wxString const & functionName, int const iLineNumber, wxString const & msg, LineType type = LineType::Error);

        private:
            static Debugger_DAP * m_dbgGDB;
    };
} //namespace dbg_DAP

#endif // _DEBUGGER_DAP_GDB_LOGGER_H_
