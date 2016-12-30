//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#include "stdafx.h"
#include "CSymbolEngineTime.h"

#include "NumericalFunctions.h"

CSymbolEngineTime *p_symbol_engine_time = NULL;

CSymbolEngineTime::CSymbolEngineTime() {
  // The values of some symbol-engines depend on other engines.
  // As the engines get later called in the order of initialization
  // we assure correct ordering by checking if they are initialized.
  //
  // This engine does not use any other engines.
  InitOnStartup();
}

CSymbolEngineTime::~CSymbolEngineTime() {
}

void CSymbolEngineTime::InitOnStartup() {
  // Initilizing all "old" values to time of startup for reasonability
  time(&_elapsedhold);
  time(&_elapsedhandhold);
  time(&_elapsedautohold);
}

void CSymbolEngineTime::UpdateOnConnection() {
  time(&_elapsedhold);
  time(&_elapsedhandhold);
  time(&_elapsedautohold);
}

void CSymbolEngineTime::UpdateOnHandreset() {
  time(&_elapsedhandhold);
}

void CSymbolEngineTime::UpdateOnNewRound() {
}

void CSymbolEngineTime::UpdateOnMyTurn() {
}

void CSymbolEngineTime::UpdateOnHeartbeat() {
  // No longer calculating time symbols on every heartbeat
  // but calculating them now on the fly
  //   a) because they are used rarely
  //   b) because they were laggy in the debug-tab (flexible heartbeat)
  //   c) because we want them to be available all the time
  //      and normal calculations stop on disconnection.
}	

void CSymbolEngineTime::UpdateOnAutoPlayerAction() {
  time(&_elapsedautohold);
}

double CSymbolEngineTime::elapsed() {
  // current time
  time_t t_now_time;
  time(&t_now_time);
  double result = t_now_time - _elapsedhold;
  assert(result < 100000);	// Heuristic: max 30 hours up-time in debug-mode
  return result;
}

double CSymbolEngineTime::elapsedhand() {
  // current time
  time_t t_now_time;
  time(&t_now_time);
  double result = t_now_time - _elapsedhandhold;
  AssertRange(result, 0, elapsed());
  return result;
}

double CSymbolEngineTime::elapsedauto() {
  // current time
  time_t t_now_time;
  time(&t_now_time);
  double result = t_now_time - _elapsedautohold;
  AssertRange(result, 0, elapsed());
  return result;
}

double CSymbolEngineTime::elapsedtoday() {
  // current time
  time_t t_now_time;
  time(&t_now_time);
  // midnight time
  tm s_midnight_time = { 0 };
  localtime_s(&s_midnight_time, &t_now_time);
  s_midnight_time.tm_hour = 0;
  s_midnight_time.tm_min = 0;
  s_midnight_time.tm_sec = 0;
  time_t t_midnight_time = mktime(&s_midnight_time);
  double result = t_now_time - t_midnight_time;
  AssertRange(result, 0, (24 * 3600));
  return result;
}


bool CSymbolEngineTime::EvaluateSymbol(const char *name, double *result, bool log /* = false */) {
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
  if (memcmp(name, "elapsed", 7)==0) {
	if (memcmp(name, "elapsed", 7)==0 && strlen(name)==7) {
      *result = elapsed();
	}
	else if (memcmp(name, "elapsedhand", 11)==0 && strlen(name)==11) {
      *result = elapsedhand();
    }
	else if (memcmp(name, "elapsedauto", 11)==0 && strlen(name)==11) {
      *result = elapsedauto();
	} else if (memcmp(name, "elapsedtoday", 12)==0 && strlen(name)==12)	{
      *result = elapsedtoday();
	} else {
	  // Invalid symbol
	   return false;
	}
	// Valid symbol
	return true;
  }
  // Symbol of name different symbol-engine
  return false;
}

CString CSymbolEngineTime::SymbolsProvided() {
  return "elapsed elapsedhand elapsedauto elapsedtoday";
}