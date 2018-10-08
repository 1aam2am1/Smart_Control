#ifndef COMMUNICATION_LOG_H
#define COMMUNICATION_LOG_H

#include <string>


class Communication_log {
public:
    Communication_log();

    virtual ~Communication_log();

    /** \brief Write function
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void write(const char *, uint32_t) = 0;

    /** \brief Write function - what we receive
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void r_write(const char *, uint32_t) = 0;

    /** \brief Write function - what we write
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void w_write(const char *, uint32_t) = 0;

protected:
    virtual std::string parse(const char *, uint32_t);

    bool new_line;
private:
};

#endif // COMMUNICATION_LOG_H
