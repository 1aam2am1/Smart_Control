#ifndef FILE_COMMUNICATION_LOG_H
#define FILE_COMMUNICATION_LOG_H

#include <Communication_log.h>
#include <string>
#include <cstdio>
#include <mutex>
#include <SFML/System/Clock.hpp>


class File_Communication_log : public Communication_log {
public:
    explicit File_Communication_log(std::string directory = "communication_log");

    virtual ~File_Communication_log();

    /** \brief Write function
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void write(const char *, uint32_t) override;

    /** \brief Write function - what we receive
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void r_write(const char *, uint32_t) override;

    /** \brief Write function - what we write
     *
     * \param String to write
     * \param How many characters
     *
     */
    virtual void w_write(const char *, uint32_t) override;

protected:
    const std::string directory;

    std::mutex mutex;
    sf::Clock clock;
    bool start;

private:
    void open();

    void time_write();

    FILE *file;
    tm open_time;
    std::string last_write;
};

#endif // FILE_COMMUNICATION_LOG_H
