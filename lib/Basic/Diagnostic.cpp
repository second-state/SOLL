#include "soll/Basic/Diagnostic.h"

namespace soll {

DiagnosticsEngine::DiagnosticsEngine(
    llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags,
    llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts,
    DiagnosticConsumer *client, bool ShouldOwnClient)
    : Diags(Diags), DiagOpts(DiagOpts), Client(client),
      Owner(ShouldOwnClient ? client : nullptr) {}

bool DiagnosticsEngine::EmitCurrentDiagnostic(bool Force) {
  assert(getClient() && "DiagnosticClient not set!");

  bool Emitted;
  if (Force) {
    Diagnostic Info(this);

    DiagnosticIDs::Level DiagLevel =
        Diags->getDiagnosticLevel(Info.getID(), Info.getLocation(), *this);

    Emitted = (DiagLevel != DiagnosticIDs::Ignored);
    if (Emitted) {
      Diags->EmitDiag(*this, DiagLevel);
    }
  } else {
    Emitted = Diags->ProcessDiag(*this);
  }

  Clear();

  if (!Force && DelayedDiagID)
    ReportDelayed();

  return Emitted;
}

void DiagnosticsEngine::setClient(DiagnosticConsumer *client,
                                  bool ShouldOwnClient) {
  Owner.reset(ShouldOwnClient ? client : nullptr);
  Client = client;
}

void DiagnosticsEngine::Report(const StoredDiagnostic &storedDiag) {
  assert(CurDiagID == std::numeric_limits<unsigned>::max() &&
         "Multiple diagnostics in flight at once!");

  CurDiagLoc = storedDiag.getLocation();
  CurDiagID = storedDiag.getID();
  NumDiagArgs = 0;

  DiagRanges.clear();
  DiagRanges.append(storedDiag.range_begin(), storedDiag.range_end());

  DiagFixItHints.clear();
  DiagFixItHints.append(storedDiag.fixit_begin(), storedDiag.fixit_end());

  assert(Client && "DiagnosticConsumer not set!");
  Level DiagLevel = storedDiag.getLevel();
  Diagnostic Info(this, storedDiag.getMessage());
  Client->HandleDiagnostic(DiagLevel, Info);
  if (Client->IncludeInDiagnosticCounts()) {
    if (DiagLevel == DiagnosticsEngine::Level::Warning)
      ++NumWarnings;
  }

  CurDiagID = std::numeric_limits<unsigned>::max();
}

void DiagnosticsEngine::ReportDelayed() {
  unsigned ID = DelayedDiagID;
  DelayedDiagID = 0;
  Report(ID) << DelayedDiagArg1 << DelayedDiagArg2;
}

} // namespace soll
