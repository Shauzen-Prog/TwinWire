#pragma once
#include <unordered_map>
#include <functional>
#include <optional>

template<typename TState, typename TOwner>
class StateMachine
{
public:
    using OnEnter = std::function<void(TOwner&)>;
    using OnUpdate = std::function<void(TOwner&, float)>; //dt
    using OnExit = std::function<void(TOwner&)>;
    using OnEvent = std::function<void(TOwner&, const void*)>;

    struct Callbacks
    {
        OnEnter enter {};
        OnUpdate update {};
        OnExit exit {};
        OnEvent event {};
    };

    explicit StateMachine(TOwner& owner) : m_owner(owner) {}

    void addState(TState s, Callbacks cbs)
    {
        m_table[s] = std::move(cbs);
    }

    void setInitial(TState s)
    {
        m_current = s;
        if (auto it = m_table.find(s); it != m_table.end() && it->second.enter)
            it->second.enter(m_owner);
    }

    TState get() const {return *m_current;}

    void change(TState s)
    {
        // Si todavia no tiene !m_current o quiere cambiar al mismo *m_currrent returnea
        if (!m_current || *m_current == s) return;

        // salir del estado actual (si tiene exit)
        if (auto it = m_table.find(s); it != m_table.end() && it->second.exit)
            it->second.exit(m_owner);

        // cambiar de estado
        m_current = s;

        // entrar al nuevo estado (si tiene enter)
        if (auto it = m_table.find(s); it != m_table.end() && it->second.enter)
            it->second.enter(m_owner);
    }

    void update(float dt) {
        if (!m_current.has_value()) return;  
        if (auto it = m_table.find(*m_current); it != m_table.end() && it->second.update)
            it->second.update(m_owner, dt);
    }

    // Despacho eventos pasandolos por punteros
    void notify(const void* evt) {
        if (!m_current) return;
        if (auto it = m_table.find(*m_current); it != m_table.end() && it->second.event)
            it->second.event(m_owner, evt);
    }

    bool has() const { return m_current.has_value(); }
    
private:
    TOwner& m_owner;
    std::optional<TState> m_current;
    std::unordered_map<TState, Callbacks> m_table;
};
