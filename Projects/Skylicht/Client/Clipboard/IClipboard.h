#pragma once

namespace Skylicht
{
	/**
	 * @brief The IClipboard interface provides methods for interacting with the system clipboard.
	 */
	class IClipboard
	{
	public:
		IClipboard();

		virtual ~IClipboard();

		/**
		 * @brief Copies text to the clipboard.
		 * @param text The text to be copied to the clipboard.
		 */
		virtual void copyToClipboard(const char* text) = 0;

		/**
		 * @brief Retrieves text from the clipboard.
		 * @return A pointer to the text retrieved from the clipboard, or nullptr if no text is available.
		 */
		virtual const char* getTextFromClipboard() = 0;
	};

	/**
	 * @brief Returns the platform-specific clipboard implementation.
	 * @return A pointer to the IClipboard instance.
	 */
	IClipboard* getOSClipboard();
}
