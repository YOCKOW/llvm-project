# TestSwiftObjCMainConflictingDylibsBridgingHeader.py
#
# This source file is part of the Swift.org open source project
#
# Copyright (c) 2018 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See https://swift.org/LICENSE.txt for license information
# See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
#
# ------------------------------------------------------------------------------

import lldb
from lldbsuite.test.lldbtest import *
from lldbsuite.test.decorators import *
import lldbsuite.test.lldbutil as lldbutil
import os
import shutil

class TestSwiftObjCMainConflictingDylibsBridgingHeader(TestBase):
    # Don't run ClangImporter tests if Clangimporter is disabled.
    @skipIf(setting=('symbols.use-swift-clangimporter', 'false'))
    @skipUnlessDarwin
    @swiftTest
    def test(self):
        self.build()
        target, process, _, _ = lldbutil.run_to_source_breakpoint(
            self, "break here",
            lldb.SBFileSpec("Bar.swift"),
            extra_images=['Foo', 'Bar'])


        self.expect("fr var bar", "expected result", substrs=["42"])
        self.expect("expression bar", "expected result", substrs=["$R0", "42"])
        self.expect("expression $R0", "expected result", substrs=["$R1", "42"])
        self.expect("expression $R1", "expected result", substrs=["$R2", "42"])
        
        foo_breakpoint = target.BreakpointCreateBySourceRegex(
            'break here', lldb.SBFileSpec('Foo.swift'))
        process.Continue()
        self.expect("fr var foo", "expected result", substrs=["23"])
        # FIXME: This should work with precise compiler invocations.
        self.expect("expression foo", "expected result", substrs=["$R3", "23"])
        self.expect("expression $R3", "expected result", substrs=["23"])
        self.expect("expression $R4", "expected result", substrs=["23"])
