"""
Test that we are able to broadcast and receive swift progress events from lldb
"""
import lldb

import lldbsuite.test.lldbutil as lldbutil

from lldbsuite.test.lldbtest import *
from lldbsuite.test.decorators import *

class TestSwiftProgressReporting(TestBase):
    def setUp(self):
        TestBase.setUp(self)
        self.broadcaster = self.dbg.GetBroadcaster()
        self.listener = lldbutil.start_listening_from(self.broadcaster,
                                        lldb.SBDebugger.eBroadcastBitProgress)

    # Don't run ClangImporter tests if Clangimporter is disabled.
    @skipIf(setting=('symbols.use-swift-clangimporter', 'false'))
    @skipUnlessDarwin
    @swiftTest
    def test_swift_progress_report(self):
        """Test that we are able to fetch swift type-system progress events"""
        self.build()

        target, process, thread, _ = lldbutil.run_to_source_breakpoint(self, 'break here',
                                          lldb.SBFileSpec('main.swift'))

        self.assertGreater(thread.GetNumFrames(), 0)
        frame = thread.GetSelectedFrame()
        self.assertTrue(frame, "Invalid frame.")

        # Resolve variable to exercise the type-system
        self.runCmd("expr boo")
        self.runCmd("v s")

        beacons = [
            "Loading Swift module",
            "Compiling bridging header",
            "Importing modules used in expression",
            "Setting up Swift reflection",
            "Importing Swift module dependencies for main.swift",
            "Importing Swift modules",
            "Importing Swift standard library",
        ]

        importing_swift_reports = []
        while len(beacons):
            event = lldbutil.fetch_next_event(self, self.listener, self.broadcaster)
            ret_args = lldb.SBDebugger.GetProgressFromEvent(event)
            if self.TraceOn():
                print(ret_args[0])

            # When importing Swift modules, make sure that we don't get two reports
            # in a row with the title "Importing Swift modules", i.e. there should be
            # a report with that title followed by a report with that title and details
            # attached.
            if ret_args[0] == "Importing Swift modules":
                next_event = lldbutil.fetch_next_event(self, self.listener, self.broadcaster)
                next_ret_args = lldb.SBDebugger.GetProgressFromEvent(next_event)
                self.assertRegexpMatches(next_ret_args[0], r"Importing Swift modules:+")

            for beacon in beacons:
                if beacon in ret_args[0]:
                    beacons.remove(beacon)
