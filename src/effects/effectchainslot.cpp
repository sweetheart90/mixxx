#include "effects/effectchainslot.h"

#include "effects/effectrack.h"
#include "control/controlpotmeter.h"
#include "control/controlpushbutton.h"
#include "mixer/playermanager.h"
#include "util/math.h"

EffectChainSlot::EffectChainSlot(EffectRack* pRack, const QString& group,
                                 unsigned int iChainNumber)
        : m_iChainSlotNumber(iChainNumber),
          // The control group names are 1-indexed while internally everything
          // is 0-indexed.
          m_group(group),
          m_pEffectRack(pRack) {
    m_pControlClear = new ControlPushButton(ConfigKey(m_group, "clear"));
    connect(m_pControlClear, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlClear(double)));

    m_pControlNumEffects = new ControlObject(ConfigKey(m_group, "num_effects"));
    m_pControlNumEffects->setReadOnly();

    m_pControlNumEffectSlots = new ControlObject(ConfigKey(m_group, "num_effectslots"));
    m_pControlNumEffectSlots->setReadOnly();

    m_pControlChainLoaded = new ControlObject(ConfigKey(m_group, "loaded"));
    m_pControlChainLoaded->setReadOnly();

    m_pControlChainEnabled = new ControlPushButton(ConfigKey(m_group, "enabled"));
    m_pControlChainEnabled->setButtonMode(ControlPushButton::POWERWINDOW);
    // Default to enabled. The skin might not show these buttons.
    m_pControlChainEnabled->setDefaultValue(true);
    m_pControlChainEnabled->set(true);
    connect(m_pControlChainEnabled, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainEnabled(double)));

    m_pControlChainMix = new ControlPotmeter(ConfigKey(m_group, "mix"), 0.0, 1.0);
    connect(m_pControlChainMix, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainMix(double)));
    m_pControlChainMix->set(1.0);

    m_pControlChainSuperParameter = new ControlPotmeter(ConfigKey(m_group, "super1"), 0.0, 1.0);
    connect(m_pControlChainSuperParameter, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainSuperParameter(double)));
    m_pControlChainSuperParameter->set(0.0);
    m_pControlChainSuperParameter->setDefaultValue(0.0);

    m_pControlChainInsertionType = new ControlPushButton(ConfigKey(m_group, "insertion_type"));
    m_pControlChainInsertionType->setButtonMode(ControlPushButton::TOGGLE);
    m_pControlChainInsertionType->setStates(EffectChain::NUM_INSERTION_TYPES);
    connect(m_pControlChainInsertionType, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainInsertionType(double)));

    m_pControlChainNextPreset = new ControlPushButton(ConfigKey(m_group, "next_chain"));
    connect(m_pControlChainNextPreset, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainNextPreset(double)));

    m_pControlChainPrevPreset = new ControlPushButton(ConfigKey(m_group, "prev_chain"));
    connect(m_pControlChainPrevPreset, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainPrevPreset(double)));

    // Ignoring no-ops is important since this is for +/- tickers.
    m_pControlChainSelector = new ControlObject(ConfigKey(m_group, "chain_selector"), false);
    connect(m_pControlChainSelector, SIGNAL(valueChanged(double)),
            this, SLOT(slotControlChainSelector(double)));

    connect(&m_channelStatusMapper, SIGNAL(mapped(const QString&)),
            this, SLOT(slotChannelStatusChanged(const QString&)));

    // ControlObjects for skin <-> controller mapping interaction.
    // Refer to comment in header for full explanation.
    m_pControlChainShowFocus = new ControlPushButton(
                                   ConfigKey(m_group, "show_focus"),
                                   true);
    m_pControlChainShowFocus->setButtonMode(ControlPushButton::TOGGLE);

    m_pControlChainShowParameters = new ControlPushButton(
                                        ConfigKey(m_group, "show_parameters"),
                                        true);
    m_pControlChainShowParameters->setButtonMode(ControlPushButton::TOGGLE);

    m_pControlChainFocusedEffect = new ControlObject(
                                       ConfigKey(m_group, "focused_effect"),
                                       true, false, true);
}

EffectChainSlot::~EffectChainSlot() {
    //qDebug() << debugString() << "destroyed";
    clear();
    delete m_pControlClear;
    delete m_pControlNumEffects;
    delete m_pControlNumEffectSlots;
    delete m_pControlChainLoaded;
    delete m_pControlChainEnabled;
    delete m_pControlChainMix;
    delete m_pControlChainSuperParameter;
    delete m_pControlChainInsertionType;
    delete m_pControlChainPrevPreset;
    delete m_pControlChainNextPreset;
    delete m_pControlChainSelector;
    delete m_pControlChainShowFocus;
    delete m_pControlChainShowParameters;
    delete m_pControlChainFocusedEffect;

    for (QMap<QString, ChannelInfo*>::iterator it = m_channelInfoByName.begin();
         it != m_channelInfoByName.end();) {
        delete it.value();
        it = m_channelInfoByName.erase(it);
    }

    m_slots.clear();
    m_pEffectChain.clear();
}

QString EffectChainSlot::id() const {
    if (m_pEffectChain)
        return m_pEffectChain->id();
    return "";
}

double EffectChainSlot::getSuperParameter() const {
    return m_pControlChainSuperParameter->get();
}

void EffectChainSlot::setSuperParameter(double value, bool force) {
    m_pControlChainSuperParameter->set(value);
    slotControlChainSuperParameter(value, force);
}

void EffectChainSlot::setSuperParameterDefaultValue(double value) {
    m_pControlChainSuperParameter->setDefaultValue(value);
}

void EffectChainSlot::slotChainNameChanged(const QString&) {
    emit(updated());
}

void EffectChainSlot::slotChainEnabledChanged(bool bEnabled) {
    m_pControlChainEnabled->set(bEnabled);
    emit(updated());
}

void EffectChainSlot::slotChainMixChanged(double mix) {
    m_pControlChainMix->set(mix);
    emit(updated());
}

void EffectChainSlot::slotChainInsertionTypeChanged(EffectChain::InsertionType type) {
    m_pControlChainInsertionType->set(static_cast<double>(type));
    emit(updated());
}

void EffectChainSlot::slotChainChannelStatusChanged(const QString& group,
                                                    bool enabled) {
    ChannelInfo* pInfo = m_channelInfoByName.value(group, NULL);
    if (pInfo != NULL && pInfo->pEnabled != NULL) {
        pInfo->pEnabled->set(enabled);
        emit(updated());
    }
}

void EffectChainSlot::slotChainEffectChanged(unsigned int effectSlotNumber,
                                             bool shouldEmit) {
    //qDebug() << debugString() << "slotChainEffectChanged" << effectSlotNumber;
    if (m_pEffectChain) {
        const QList<EffectPointer> effects = m_pEffectChain->effects();
        EffectSlotPointer pSlot;
        EffectPointer pEffect;

        if (effects.size() > m_slots.size()) {
            qWarning() << debugString() << "has too few slots for effect";
        }

        if (effectSlotNumber < (unsigned) m_slots.size()) {
            pSlot = m_slots.at(effectSlotNumber);
        }
        if (effectSlotNumber < (unsigned) effects.size()) {
            pEffect = effects.at(effectSlotNumber);
        }
        if (pSlot != nullptr) {
            pSlot->loadEffect(pEffect);
        }

        m_pControlNumEffects->forceSet(math_min(
            static_cast<unsigned int>(m_slots.size()),
            m_pEffectChain->numEffects()));

        if (shouldEmit) {
            emit(updated());
        }
    }
}

void EffectChainSlot::loadEffectChain(EffectChainPointer pEffectChain) {
    //qDebug() << debugString() << "loadEffectChain" << (pEffectChain ? pEffectChain->id() : "(null)");
    clear();

    if (pEffectChain) {
        m_pEffectChain = pEffectChain;
        m_pEffectChain->addToEngine(m_pEffectRack->getEngineEffectRack(),
                                    m_iChainSlotNumber);
        m_pEffectChain->updateEngineState();

        connect(m_pEffectChain.data(), SIGNAL(effectChanged(unsigned int)),
                this, SLOT(slotChainEffectChanged(unsigned int)));
        connect(m_pEffectChain.data(), SIGNAL(nameChanged(const QString&)),
                this, SLOT(slotChainNameChanged(const QString&)));
        connect(m_pEffectChain.data(), SIGNAL(enabledChanged(bool)),
                this, SLOT(slotChainEnabledChanged(bool)));
        connect(m_pEffectChain.data(), SIGNAL(mixChanged(double)),
                this, SLOT(slotChainMixChanged(double)));
        connect(m_pEffectChain.data(), SIGNAL(insertionTypeChanged(EffectChain::InsertionType)),
                this, SLOT(slotChainInsertionTypeChanged(EffectChain::InsertionType)));
        connect(m_pEffectChain.data(), SIGNAL(channelStatusChanged(const QString&, bool)),
                this, SLOT(slotChainChannelStatusChanged(const QString&, bool)));

        m_pControlChainLoaded->forceSet(true);
        m_pControlChainInsertionType->set(m_pEffectChain->insertionType());

        // Mix and enabled channels are persistent properties of the chain slot,
        // not of the chain. Propagate the current settings to the chain.
        m_pEffectChain->setMix(m_pControlChainMix->get());
        m_pEffectChain->setEnabled(m_pControlChainEnabled->get() > 0.0);
        foreach (ChannelInfo* pChannelInfo, m_channelInfoByName) {
            if (pChannelInfo->pEnabled->toBool()) {
                m_pEffectChain->enableForChannel(pChannelInfo->handle_group);
            } else {
                m_pEffectChain->disableForChannel(pChannelInfo->handle_group);
            }
        }

        // Don't emit because we will below.
        for (int i = 0; i < m_slots.size(); ++i) {
            slotChainEffectChanged(i, false);
        }
    }

    emit(effectChainLoaded(pEffectChain));
    emit(updated());
}

EffectChainPointer EffectChainSlot::getEffectChain() const {
    return m_pEffectChain;
}

void EffectChainSlot::clear() {
    // Stop listening to signals from any loaded effect
    if (m_pEffectChain) {
        m_pEffectChain->removeFromEngine(m_pEffectRack->getEngineEffectRack(),
                                         m_iChainSlotNumber);
        foreach (EffectSlotPointer pSlot, m_slots) {
            pSlot->clear();
        }
        m_pEffectChain->disconnect(this);
        m_pEffectChain.clear();
    }
    m_pControlNumEffects->forceSet(0.0);
    m_pControlChainLoaded->forceSet(0.0);
    m_pControlChainInsertionType->set(EffectChain::INSERT);
    emit(updated());
}

unsigned int EffectChainSlot::numSlots() const {
    //qDebug() << debugString() << "numSlots";
    return m_slots.size();
}

EffectSlotPointer EffectChainSlot::addEffectSlot(const QString& group) {
    //qDebug() << debugString() << "addEffectSlot" << group;

    EffectSlot* pEffectSlot = new EffectSlot(group, m_iChainSlotNumber,
                                             m_slots.size());
    // Rebroadcast effectLoaded signals
    connect(pEffectSlot, SIGNAL(effectLoaded(EffectPointer, unsigned int)),
            this, SLOT(slotEffectLoaded(EffectPointer, unsigned int)));
    connect(pEffectSlot, SIGNAL(clearEffect(unsigned int)),
            this, SLOT(slotClearEffect(unsigned int)));
    connect(pEffectSlot, SIGNAL(nextEffect(unsigned int, unsigned int, EffectPointer)),
            this, SIGNAL(nextEffect(unsigned int, unsigned int, EffectPointer)));
    connect(pEffectSlot, SIGNAL(prevEffect(unsigned int, unsigned int, EffectPointer)),
            this, SIGNAL(prevEffect(unsigned int, unsigned int, EffectPointer)));

    EffectSlotPointer pSlot(pEffectSlot);
    m_slots.append(pSlot);
    m_pControlNumEffectSlots->forceSet(m_pControlNumEffectSlots->get() + 1);
    return pSlot;
}

void EffectChainSlot::registerChannel(const ChannelHandleAndGroup& handle_group) {
    if (m_channelInfoByName.contains(handle_group.name())) {
        qWarning() << debugString()
                   << "WARNING: registerChannel already has channel registered:"
                   << handle_group.name();
        return;
    }

    double initialValue = 0.0;
    int deckNumber;
    if (PlayerManager::isDeckGroup(handle_group.name(), &deckNumber) &&
        (m_iChainSlotNumber + 1) == (unsigned) deckNumber) {
        initialValue = 1.0;
    }
    ControlPushButton* pEnableControl = new ControlPushButton(
            ConfigKey(m_group, QString("group_%1_enable").arg(handle_group.name())),
            true, initialValue);
    pEnableControl->setButtonMode(ControlPushButton::POWERWINDOW);

    ChannelInfo* pInfo = new ChannelInfo(handle_group, pEnableControl);
    m_channelInfoByName[handle_group.name()] = pInfo;
    m_channelStatusMapper.setMapping(pEnableControl, handle_group.name());
    connect(pEnableControl, SIGNAL(valueChanged(double)),
            &m_channelStatusMapper, SLOT(map()));

    if (m_pEffectChain != nullptr) {
        if (pEnableControl->toBool()) {
            m_pEffectChain->enableForChannel(handle_group);
        } else {
            m_pEffectChain->disableForChannel(handle_group);
        }
    }
}

void EffectChainSlot::slotEffectLoaded(EffectPointer pEffect, unsigned int slotNumber) {
    // const int is a safe read... don't bother locking
    emit(effectLoaded(pEffect, m_iChainSlotNumber, slotNumber));
}

void EffectChainSlot::slotClearEffect(unsigned int iEffectSlotNumber) {
    if (m_pEffectChain) {
        m_pEffectChain->removeEffect(iEffectSlotNumber);
    }
}

EffectSlotPointer EffectChainSlot::getEffectSlot(unsigned int slotNumber) {
    //qDebug() << debugString() << "getEffectSlot" << slotNumber;
    if (slotNumber >= static_cast<unsigned int>(m_slots.size())) {
        qWarning() << "WARNING: slotNumber out of range";
        return EffectSlotPointer();
    }
    return m_slots[slotNumber];
}

void EffectChainSlot::slotControlClear(double v) {
    if (v > 0) {
        clear();
    }
}

void EffectChainSlot::slotControlChainEnabled(double v) {
    //qDebug() << debugString() << "slotControlChainEnabled" << v;
    if (m_pEffectChain) {
        m_pEffectChain->setEnabled(v > 0);
    }
}

void EffectChainSlot::slotControlChainMix(double v) {
    //qDebug() << debugString() << "slotControlChainMix" << v;

    // Clamp to [0.0, 1.0]
    if (v < 0.0 || v > 1.0) {
        qWarning() << debugString() << "value out of limits";
        v = math_clamp(v, 0.0, 1.0);
        m_pControlChainMix->set(v);
    }
    if (m_pEffectChain) {
        m_pEffectChain->setMix(v);
    }
}

void EffectChainSlot::slotControlChainSuperParameter(double v, bool force) {
    //qDebug() << debugString() << "slotControlChainSuperParameter" << v;

    // Clamp to [0.0, 1.0]
    if (v < 0.0 || v > 1.0) {
        qWarning() << debugString() << "value out of limits";
        v = math_clamp(v, 0.0, 1.0);
        m_pControlChainSuperParameter->set(v);
    }
    for (const auto& pSlot : m_slots) {
        pSlot->setMetaParameter(v, force);
    }
}

void EffectChainSlot::slotControlChainInsertionType(double v) {
    // Intermediate cast to integer is needed for VC++.
    EffectChain::InsertionType type = static_cast<EffectChain::InsertionType>(int(v));
    (void)v; // this avoids a false warning with g++ 4.8.1
    if (m_pEffectChain && type >= 0 &&
            type < EffectChain::NUM_INSERTION_TYPES) {
        m_pEffectChain->setInsertionType(type);
    }
}

void EffectChainSlot::slotControlChainSelector(double v) {
    //qDebug() << debugString() << "slotControlChainSelector" << v;
    if (v > 0) {
        emit(nextChain(m_iChainSlotNumber, m_pEffectChain));
    } else if (v < 0) {
        emit(prevChain(m_iChainSlotNumber, m_pEffectChain));
    }
}

void EffectChainSlot::slotControlChainNextPreset(double v) {
    //qDebug() << debugString() << "slotControlChainNextPreset" << v;
    if (v > 0) {
        slotControlChainSelector(1);
    }
}

void EffectChainSlot::slotControlChainPrevPreset(double v) {
    //qDebug() << debugString() << "slotControlChainPrevPreset" << v;
    if (v > 0) {
        slotControlChainSelector(-1);
    }
}

void EffectChainSlot::slotChannelStatusChanged(const QString& group) {
    if (m_pEffectChain) {
        ChannelInfo* pChannelInfo = m_channelInfoByName.value(group, NULL);
        if (pChannelInfo != NULL && pChannelInfo->pEnabled != NULL) {
            bool bEnable = pChannelInfo->pEnabled->toBool();
            if (bEnable) {
                m_pEffectChain->enableForChannel(pChannelInfo->handle_group);
            } else {
                m_pEffectChain->disableForChannel(pChannelInfo->handle_group);
            }
        }
    }
}

unsigned int EffectChainSlot::getChainSlotNumber() const {
    return m_iChainSlotNumber;
}
