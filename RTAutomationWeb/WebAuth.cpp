////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTAutomation
//
//  Copyright (c) 2015-2016, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef WIN32
#include <unistd.h>
#endif

#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"
#include "Wt/Auth/GoogleService"
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"

#include <Wt/WApplication>

#include "WebAuth.h"

#include <qdebug.h>

using namespace Wt;
namespace dbo = Wt::Dbo;

Auth::AuthService WebAuthService;
Auth::PasswordService WebPasswordService(WebAuthService);

void WebAuth::configureAuth()
{
    WebAuthService.setAuthTokensEnabled(true, "WebCookie");
    WebAuthService.setEmailVerificationEnabled(true);

    Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
    verifier->addHashFunction(new Auth::BCryptHashFunction(7));

    WebPasswordService.setVerifier(verifier);
    WebPasswordService.setStrengthValidator(new Auth::PasswordStrengthValidator());
    WebPasswordService.setAttemptThrottlingEnabled(true);
}

WebAuth::WebAuth() : m_sqlite3(WApplication::instance()->appRoot() + "RTAutomationWeb.db")
{
    m_authSession.setConnection(m_sqlite3);
    m_sqlite3.setProperty("show-queries", "true");

    m_authSession.mapClass<WebUser>("user");
    m_authSession.mapClass<AuthInfo>("auth_info");
    m_authSession.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    m_authSession.mapClass<AuthInfo::AuthTokenType>("auth_token");

    m_users = new UserDatabase(m_authSession);

    dbo::Transaction transaction(m_authSession);
    try
    {
        m_authSession.createTables();

        Auth::User guestUser = m_users->registerNew();
        guestUser.addIdentity(Auth::Identity::LoginName, "richards-tech");
        WebPasswordService.updatePassword(guestUser, "richards-tech");

        qDebug() << "Database created";
    } catch (...) {
        qDebug() << "Using existing database";
    }

    transaction.commit();
}

WebAuth::~WebAuth()
{
  delete m_users;
}

Auth::AbstractUserDatabase& WebAuth::users()
{
  return *m_users;
}

const Auth::AuthService& WebAuth::auth()
{
  return WebAuthService;
}

const Auth::AbstractPasswordService& WebAuth::passwordAuth()
{
  return WebPasswordService;
}
