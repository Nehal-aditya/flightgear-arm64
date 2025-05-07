// dialog.cxx: implementation of an XML-configurable dialog box.

#include "config.h"

#include <utility>

#include "dialog.hxx"

#include <simgear/props/props.hxx>

namespace {

using WS = FGDialog::WindowStyle;
using WF = FGDialog::WindowFlags;

FGDialog::WindowStyle styleFromProps(const std::string& s)
{
    if (s == "modal-dialag") {
        return WS::ModalDialog;
    }

    if (s == "message-box") {
        return WS::MessageBox;
    }

    return WS::Window;
}

int defaultFlagsForStyle(FGDialog::WindowStyle ws)
{
    switch (ws) {
    case WS::ModalDialog:
        return WF::ButtonBox;

    case WS::MessageBox:
        return WF::ButtonBox;

    default:
        return WF::Resizable | WF::Closeable;
    }
}

} // namespace

FGDialog::FGDialog(SGPropertyNode* props, std::string translationDomain)
    : _translationDomain(std::move(translationDomain)),
      _windowStyle(styleFromProps(props->getStringValue("window-style")))
{
    _flags = defaultFlagsForStyle(_windowStyle);
    updateFlagFromProperty(WF::Closeable, props, "closeable");
    updateFlagFromProperty(WF::Resizable, props, "resizeable");
    updateFlagFromProperty(WF::ButtonBox, props, "has-buttons");

    const auto translationDomainNode = props->getChild("translation-domain");
    if (translationDomainNode) {
        // Override what was set by the constructor member initializer list
        setTranslationDomain(translationDomainNode->getStringValue());
    }
}

std::string FGDialog::translationDomain() const noexcept
{
    return _translationDomain;
}

void FGDialog::setTranslationDomain(std::string domain) noexcept
{
    _translationDomain = std::move(domain);
}

void FGDialog::updateFlagFromProperty(WindowFlags f, SGPropertyNode* props, const std::string& name)
{
    auto c = props->getChild(name);
    if (!c) {
        return;
    }

    const auto invF = ~f;
    _flags &= invF; // clear to zero
    if (c->getBoolValue()) {
        _flags |= f;
    }
}


FGDialog::~FGDialog() = default;

FGDialog::WindowStyle FGDialog::windowStyle() const
{
    return _windowStyle;
}

bool FGDialog::isFlagSet(WindowFlags f) const
{
    return _flags & f;
}
