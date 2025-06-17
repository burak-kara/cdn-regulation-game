# Import WebClient from Python SDK (github.com/slackapi/python-slack-sdk)

import sys
from slack_sdk import WebClient
from slack_sdk.errors import SlackApiError

# WebClient instantiates a client that can call API methods
# When using Bolt, you can use either `app.client` or the `client` passed to listeners.
client = WebClient(token="XXXXXXXXXXXXXXXXXXXXXXXXXX") # Replace with your Slack Bot User OAuth Token
channel_name = "xxxxxxxxxx"  # Replace with your channel name
channel_id = None
days, hours, minutes, seconds, raw = None, None, None, None, None
game_modes = list()

tax_mode = sys.argv[1]


def find_channel_id() -> None:
    global channel_id
    try:
        # Call the conversations.list method using the WebClient
        for _result in client.conversations_list():
            if channel_id is not None:
                break
            for channel in _result["channels"]:
                if channel["name"] == channel_name:
                    channel_id = channel["id"]
                    break

    except SlackApiError as e:
        print(f"Error finding channel id: {e}")


def read_log():
    global days, hours, minutes, seconds, raw, game_modes

    with open("logs/log", "r") as _file:
        for _line in _file:
            if "Error:" in _line:
                raise Exception(_line)
            if "Excel Friendly Execution Time:" in _line:
                _execution_time = _line.split("Excel Friendly Execution Time:")[1].strip()
                split = _execution_time.split(":")
                days = int(split[0])
                hours = int(split[1])
                minutes = int(split[2])
                seconds = int(split[3])
                raw = _execution_time
            if "Game Mode:" in _line:
                game_modes.append(_line.split("Game Mode:")[1].strip())
    if days is None or hours is None or minutes is None or seconds is None:
        raise Exception("Execution Time not found! Another error occurred, check log file.")


def send_finish_message():
    try:
        client.chat_postMessage(
            channel=channel_id,
            blocks=[
                {
                    "type": "header",
                    "text": {
                        "type": "plain_text",
                        "text": ":finish_flag: Job Finished :tada:",
                        "emoji": True
                    }
                },
                {
                    "type": "section",
                    "text": {
                        "type": "plain_text",
                        "text": f"Duration: {days} days, {hours} hours, {minutes} minutes, {seconds} seconds."
                    }
                },
                {
                    "type": "section",
                    "text": {
                        "type": "mrkdwn",
                        "text": f"Raw: ```{raw}```"
                    }
                },
                {
                    "type": "section",
                    "text": {
                        "type": "plain_text",
                        "text": f"Game Modes: {', '.join(game_modes)}"
                    }
                },
                {
                    "type": "section",
                    "text": {
                        "type": "plain_text",
                        "text": f"Tax Mode: {tax_mode}"
                    }
                },
            ],
            text="Job Finished! :tada:"
        )
        print("Message sent successfully")
    except SlackApiError as e:
        print(f"Error sending finish message: {e}")


def send_error_message(_error: str):
    try:
        client.chat_postMessage(
            channel=channel_id,
            blocks=[
                {
                    "type": "header",
                    "text": {
                        "type": "plain_text",
                        "text": ":fail: Error :alert:",
                        "emoji": True
                    }
                },
                {
                    "type": "section",
                    "text": {
                        "type": "mrkdwn",
                        "text": f"Error: {_error}"
                    }
                },
            ],
            text="Error! :alert:"
        )
        print("Error message sent successfully")
    except SlackApiError as e:
        print(f"Error sending error message: {e}")


def main():
    try:
        find_channel_id()
        read_log()
        send_finish_message()
    except Exception as e:
        print(f"Error on notification read_log: {e}")
        send_error_message(str(e))


if __name__ == '__main__':
    main()
