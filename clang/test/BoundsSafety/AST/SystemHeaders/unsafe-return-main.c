#include <unsafe-return-sys.h>

// RUN: %clang_cc1 -ast-dump -fbounds-safety %s -verify -I %S/include | FileCheck --check-prefixes RELAXED %S/include/unsafe-return-sys.h
// RUN: %clang_cc1 -ast-dump -fbounds-safety %s -verify -I %S/include -x objective-c -fbounds-attributes-objc-experimental | FileCheck --check-prefixes RELAXED %S/include/unsafe-return-sys.h
// expected-no-diagnostics
//
// RUN: %clang_cc1 -ast-dump -fbounds-safety %s -verify=strict -fno-bounds-safety-relaxed-system-headers -I %S/include | FileCheck --check-prefixes STRICT,MAINCHECK %S/include/unsafe-return-sys.h
// RUN: %clang_cc1 -ast-dump -fbounds-safety %s -verify=strict -fno-bounds-safety-relaxed-system-headers -I %S/include -x objective-c -fbounds-attributes-objc-experimental | FileCheck --check-prefixes STRICT,MAINCHECK %S/include/unsafe-return-sys.h

void func(int * __unsafe_indexable unsafe, int * __terminated_by(2) term) {
  funcInSDK(unsafe);
  funcInSDK2(term);
  funcInSDK3(unsafe);
  funcInSDK4();
  funcInSDK5();
  funcInSDK6();
}

