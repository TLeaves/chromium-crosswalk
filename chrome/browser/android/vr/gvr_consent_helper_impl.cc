// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/android/vr/gvr_consent_helper_impl.h"

#include <utility>

#include "base/android/jni_string.h"
#include "base/callback.h"
#include "base/macros.h"
#include "chrome/android/features/vr/jni_headers/VrConsentDialog_jni.h"
#include "chrome/browser/android/tab_android.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"

using base::android::AttachCurrentThread;
using base::android::ScopedJavaLocalRef;

namespace vr {

namespace {

base::android::ScopedJavaLocalRef<jobject> GetTabFromRenderer(
    int render_process_id,
    int render_frame_id) {
  content::RenderFrameHost* render_frame_host =
      content::RenderFrameHost::FromID(render_process_id, render_frame_id);
  DCHECK(render_frame_host);

  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(render_frame_host);
  DCHECK(web_contents);

  TabAndroid* tab_android = TabAndroid::FromWebContents(web_contents);
  DCHECK(tab_android);

  base::android::ScopedJavaLocalRef<jobject> j_tab_android =
      tab_android->GetJavaObject();
  DCHECK(!j_tab_android.is_null());

  return j_tab_android;
}

}  // namespace

GvrConsentHelperImpl::GvrConsentHelperImpl() : weak_ptr_(this) {}

GvrConsentHelperImpl::~GvrConsentHelperImpl() = default;

void GvrConsentHelperImpl::PromptUserAndGetConsent(
    int render_process_id,
    int render_frame_id,
    OnUserConsentCallback on_user_consent_callback) {
  DCHECK(!on_user_consent_callback_);
  on_user_consent_callback_ = std::move(on_user_consent_callback);
  render_process_id_ = render_process_id;
  render_frame_id_ = render_frame_id;

  JNIEnv* env = AttachCurrentThread();
  jdelegate_ = Java_VrConsentDialog_promptForUserConsent(
      env, reinterpret_cast<jlong>(this),
      GetTabFromRenderer(render_process_id_, render_frame_id_));
  if (jdelegate_.is_null()) {
    std::move(on_user_consent_callback_).Run(false);
    return;
  }
}

void GvrConsentHelperImpl::OnUserConsentResult(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_caller,
    jboolean is_granted) {
  jdelegate_.Reset();
  if (!on_user_consent_callback_)
    return;

  if (!is_granted) {
    std::move(on_user_consent_callback_).Run(false);
    return;
  }

  InitModule();
}

void GvrConsentHelperImpl::InitModule() {
  if (!module_delegate_) {
    module_delegate_ = VrModuleProviderFactory::CreateModuleProvider(
        render_process_id_, render_frame_id_);
  }

  if (!module_delegate_) {
    std::move(on_user_consent_callback_).Run(false);
    return;
  }

  if (!module_delegate_->ModuleInstalled()) {
    module_delegate_->InstallModule(base::BindOnce(
        &GvrConsentHelperImpl::OnModuleInstalled, weak_ptr_.GetWeakPtr()));
    return;
  }

  std::move(on_user_consent_callback_).Run(true);
}

void GvrConsentHelperImpl::OnModuleInstalled(bool success) {
  if (!success) {
    std::move(on_user_consent_callback_).Run(false);
    return;
  }

  std::move(on_user_consent_callback_).Run(true);
}

}  // namespace vr
