#pragma once

#include <optional>
#include <string>
#include <vector>

namespace util {

    template <typename Ok, typename Err>
    struct Result {
        std::optional<Ok> ok;
        std::optional<Err> err;

        Result() = default;

        Result(const Ok& ok) : ok(ok) {}
        Result(const Err& err) : err(err) {}

        Result(const Result<Ok, Err>& other) : ok(other.ok), err(other.err) {}

        Result(Result<Ok, Err>&& other) : ok(std::move(other.ok)), err(std::move(other.err)) {}

        Result<Ok, Err>& operator=(const Result<Ok, Err>& other) {
            ok = other.ok;
            err = other.err;

            return *this;
        }

        Result<Ok, Err>& operator=(Result<Ok, Err>&& other) {
            ok = std::move(other.ok);
            err = std::move(other.err);

            return *this;
        }

        Result<Ok, Err>& operator=(Ok& ok) {
            this->ok = ok;
            this->err = std::nullopt;

            return *this;
        }

        Result<Ok, Err>& operator=(Err& err) {
            this->err = err;
            this->ok = std::nullopt;

            return *this;
        }

        bool is_ok() const {
            return ok.has_value();
        }

        bool is_err() const {
            return err.has_value();
        }

        const Ok& unwrap() const {
            return ok.value();
        }

        const Err& unwrap_err() const {
            return err.value();
        }

        Ok& unwrap() {
            return ok.value();
        }

        Err& unwrap_err() {
            return err.value();
        }

        template <typename T>
        const Ok& unwrap_or(const T& other) const {
            if (ok.has_value()) {
                return ok.value();
            } else {
                return other;
            }
        }
    };

    class Error {
    public:
        explicit Error(std::string message) : m_message(std::move(message)) {}

        [[nodiscard]] const std::string& get_message() const {
            return m_message;
        }

    private:
        std::string m_message;
    };

    template <typename Ok>
    using ResultOk = Result<Ok, Error>;

    template <typename Ok>
    using Results = Result<Ok, std::vector<Error>>;

}