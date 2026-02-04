#include <memory>
#include <type_traits>
#include <utility>

namespace std
{
    template <typename>
    class move_only_function;

    template <typename R, typename... Args>
    class move_only_function<R(Args...)> 
    {
        struct Concept {
            virtual ~Concept() = default;
            virtual R invoke(Args&&...) = 0;
            virtual std::unique_ptr<Concept> clone() = 0;
        };

        template <typename F>
        struct Model final : Concept {
            F f;

            template <typename G>
            Model(G&& g) : f(std::forward<G>(g)) {}

            R invoke(Args&&... args) override {
                return std::invoke(f, std::forward<Args>(args)...);
            }

            std::unique_ptr<Concept> clone() override {
                return std::make_unique<Model>(f);
            }
        };

        std::unique_ptr<Concept> impl;

    public:
        move_only_function() noexcept = default;
        move_only_function(std::nullptr_t) noexcept {}

        template <typename F>
        move_only_function(F&& f) {
            if constexpr (std::is_invocable_r_v<R, F, Args...>) {
                impl = std::make_unique<Model<std::decay_t<F>>>(std::forward<F>(f));
            }
        }

        // Move operations
        move_only_function(move_only_function&&) noexcept = default;
        move_only_function& operator=(move_only_function&&) noexcept = default;

        // No copy operations
        move_only_function(const move_only_function&) = delete;
        move_only_function& operator=(const move_only_function&) = delete;

        explicit operator bool() const noexcept {
            return static_cast<bool>(impl);
        }

        R operator()(Args... args) const {
            if (!impl) {
                throw std::bad_function_call();
            }
            return impl->invoke(std::forward<Args>(args)...);
        }

        void swap(move_only_function& other) noexcept {
            impl.swap(other.impl);
        }

        friend void swap(move_only_function& lhs, move_only_function& rhs) noexcept {
            lhs.swap(rhs);
        }
    };
}
