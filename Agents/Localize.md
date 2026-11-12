# Localization and GUI text

This guide follows [FirstInit.md](FirstInit.md) and describes translation lookup, `CCanvas::OnLocalize`, `CGUIText`, and fonts for different writing systems. Examples use public engine/SampleFW APIs and illustrative application assets; no private project code or assets are required.

## 1. Separate translations, GUI text, and fonts

| Responsibility | API or owner |
| --- | --- |
| Load translation data and select a language | SampleFW `CLocalize` |
| Associate a GUI label with a translation key | `CGUIText::setTextId` or the imported GUI text ID |
| Apply translation and font policy | Application callback assigned to `CCanvas::OnLocalize` |
| Refresh labels already on screen | `CCanvas::updateLocalizedText()` |
| Render characters | `CFontSource`, `IFont`, and `CGUIText` |

`CLocalize` is a SampleFW helper, not a core engine translation service. Include the public `Samples/SampleFW/Localize` and its `Config` dependency in your application module, with the appropriate include roots, or adapt that helper into your own configuration layer. Do not compile another application's localization class alongside it with the same global class name.

The checked-in SampleFW version supports only `ELanguage::EN` and `ELanguage::VN`. It loads CSV through `CBaseConfig::init(const char*)`; its record key is `STRING_ID`. Additional languages require extending the enum, `CDataText` properties/constructor, `getString` selection, and the CSV columns together. Do not assume every production application's localization helper has the same languages, loader, or argument order.

## 2. Initialize translation data before importing localized GUI

The public [GUI sample](../Samples/GUI/Source/CViewInit.cpp) loads [Text.csv](../Assets/SampleGUIDemo/Configs/Text.csv). An independent application can provide a UTF-8 CSV with this schema:

```csv
STRING_ID,LANG_EN,LANG_VN
TXT_PLAY,Play,Play
TXT_SETTINGS,Settings,Settings
```

Replace the example values with actual translations. Follow the CSV parser's quoting rules for commas, quotes, and line breaks, and keep keys stable and unique. Do not pass a binary `.xlsx` file to this SampleFW `init()` API.

```cpp
// During application/service startup, once:
CLocalize::createGetInstance();
CLocalize* localize = CLocalize::getInstance();
localize->init("UI/Configs/Text.csv"); // Illustrative application asset.
localize->setLanguage(ELanguage::EN);
```

Mount the data archive or make its loose path accessible first. Initialize once per owned data lifetime; the current loader does not clear previous records before reinitialization. Validate the loaded keys explicitly because `init()` returns no success flag and a missing CSV can leave lookups empty.

Public SampleFW lookups are:

```cpp
const char* current = CLocalize::get("TXT_PLAY");
const char* english = CLocalize::get(ELanguage::EN, "TXT_PLAY");
```

The explicit-language overload takes language **first**. Unknown IDs return an empty string, and an empty language cell has no automatic fallback to English. Define a fallback policy in the application. Returned text is borrowed from localization data: copy it into an application-owned `std::string` if it must survive a data reload or service shutdown.

## 3. Register a canvas callback before GUI import

Assume an application view owns `m_canvas` and defines `onLocalize(CGUIText*)`:

```cpp
m_canvas->OnLocalize = [this](CGUIText* text)
{
	onLocalize(text);
};
CGUIImporter::loadGUI("UI/Menu.gui", m_canvas);
m_canvas->updateEntities();
```

Include `Graphics2D/CGUIImporter.h`, `Graphics2D/CCanvas.h`, `Graphics2D/GUI/CGUIText.h`, and the application's `Localize/CLocalize.h` as needed. GUI text deserialization invokes `OnLocalize` after loading text properties. Register the callback first so imported labels get both their initial translations and their locale fonts.

For text created in code, `setTextId()` only stores the key; it does not invoke localization:

```cpp
label->setDefaultText("Play");
label->setTextId("TXT_PLAY");
onLocalize(label);
```

Use the UTF-8 `const char*` overload in these examples. `setText(const char*)` accepts UTF-8 and converts it internally for rendering. `setText()` changes the displayed value but does not change its translation key. A dynamic label with a stale text ID can therefore be overwritten on the next localization refresh.

## 4. Apply font policy, then translation

The following method belongs to an illustrative `CMenuView`. Its application-defined `applyLocaleFont(text)` selects or clears a custom font as described in the next section. Copy input text before calling text setters, since getters expose the element's own string storage.

```cpp
void CMenuView::onLocalize(CGUIText* text)
{
	applyLocaleFont(text);

	std::string id = text->getTextId();
	std::string value;
	if (!id.empty())
	{
		value = CLocalize::get(id.c_str());
		if (value.empty())
			value = CLocalize::get(ELanguage::EN, id.c_str());
		if (value.empty())
			value = text->getDefaultText();
		if (value.empty())
			value = id;
	}
	else
	{
		// Preserve dynamic values, names, and counters without text IDs.
		value = text->getText();
		if (value.empty())
			value = text->getDefaultText();
	}

	text->setText(value.c_str());
}
```

This fallback policy is an application choice, not built into SampleFW. An intentionally blank translation needs an explicit convention if empty strings otherwise trigger fallback. Keep formatted dynamic values separate from static keyed labels, and refresh their format templates/values after a language change. Prefer `UI::CUIButton::setLabel(...)` for button text when using that wrapper.

`CGUIText::setTextStrim(...)` is the engine's width-trimming API (the spelling is intentional); `isStrim()` exposes the imported setting. It measures with the current resolved font, which may not yet reflect a newly assigned custom font, and it falls back to `setText()` if no font is resolved. For labels that require trimming, resolve/update the new font and layout first, then reapply trimming from the full translated string. Do not repeatedly trim the already shortened display text. Adjust width, line spacing, wrapping, and alignment for each locale as needed.

## 5. Select fonts for different writing systems

A translation is only readable if the font contains its glyphs. Check Vietnamese diacritics and Cyrillic as well as Korean, Japanese, Chinese, and Thai. Font coverage and language support are application choices; the public SampleFW enum must be extended before referring to languages beyond EN/VN.

Use `CGUIText::getFontData()` to identify the original GUI font source, then `setCustomFont(replacement->getFont())` to override rendering without replacing that source. On returning to a locale supported by the original font, call `setCustomFont(NULL)`. This preserves the designed font identity and avoids treating the previous locale's replacement as the base for the next replacement.

A locale font policy can map a writing system and original style to a packaged TTF/OTF font:

| Locale/script requirement | Font policy |
| --- | --- |
| Latin, Vietnamese, Cyrillic | Keep original font only if its glyph coverage is sufficient |
| Korean / Japanese / Chinese | Select a matching regional font with the required glyph set |
| Thai | Select a Thai-capable font and verify mark placement and line height |
| Numeric or icon-only text | Explicitly preserve its specialized font where appropriate |

Use named application metadata or an explicit set of excluded labels for the last case; do not embed unexplained numeric tag values in a reusable engine guide. A font override does not guarantee shaping, bidirectional layout, or script-specific line breaking. Verify those behaviors with real translated strings in the current renderer rather than assuming TTF support provides them automatically.

### Create and cache a replacement font source

The application should own replacement `CFontSource` objects. For example, a method in an application-defined font policy can create a replacement after selecting its path and size:

```cpp
CFontSource* replacement = new CFontSource();
replacement->FontType.set(CFontSource::GlyphFreeType);
replacement->Source.set("UI/Fonts/Locale-Regular.ttf");
replacement->FontSizePt.set(original->FontSizePt.get());
if (!replacement->initFont())
{
	delete replacement;
	// Report missing/invalid font and use a validated fallback.
	return;
}
text->setCustomFont(replacement->getFont());
// Store replacement in the application's font cache; do not delete it here.
```

Here `original` is the non-null result of `text->getFontData()` and the font asset path is illustrative. Initialize the font subsystem and mount the font assets before this callback can run. Check load failure before installing an override; retain a known usable font or show an explicit missing-glyph diagnostic.

Cache by original source/style, destination font path, and effective point size so repeated localization callbacks reuse the same font. A cache keyed by original `CFontSource*` also requires that those source objects outlive its entries; clear or rebuild the cache before original sources are destroyed/reloaded. Preserve weight/style as well as size when choosing replacements. Optional size multipliers are per-font tuning, not universal language constants.

Do not mutate the shared original `CFontSource` just to change one label's language: other labels can use it too. Do not allocate a new replacement on every frame or every callback. `setCustomFont()` stores a raw font pointer without taking an owning reference; keep its `CFontSource` alive while any label or queued rendering uses that font.

The application-defined `applyLocaleFont` should follow this sequence:

1. Read the selected application locale and the label's original font/style.
2. If the label is exempt, preserve its intended specialized font.
3. If the locale needs a replacement, obtain a validated cached source and set its custom font.
4. Otherwise clear the prior locale override with `setCustomFont(NULL)`.
5. Apply the full translated text and refresh layout/measurement when needed.

Resolve a replacement independently of the currently installed custom font so transitions such as Japanese -> Thai -> English do not reuse the wrong font. Define a deterministic fallback when a required font is unavailable instead of silently keeping the previous locale's font.

## 6. Change language while screens are open

> **Refresh after a language change:** Call `canvas->updateLocalizedText()` on every affected canvas after `CLocalize::setLanguage(...)`. This invokes the registered `OnLocalize` callback again to reapply translated text and locale fonts. Call it again when restoring the previous language; `setLanguage()` alone does not refresh GUI.

`CLocalize::setLanguage()` only assigns an enum value. It neither broadcasts a language-change event nor updates existing labels or fonts.

```cpp
CLocalize::getInstance()->setLanguage(ELanguage::VN);
m_canvas->updateLocalizedText();
// Refresh application-owned formatted counters, button labels, and layout.
```

`updateLocalizedText()` traverses GUI descendants beneath the canvas root, invokes `OnLocalize` for each `CGUIText`, and does nothing if no callback is bound. Apply it to every live canvas that must change, including popups, overlays, and hidden screens that may reappear. Refresh font policy even for unkeyed text such as player names and numbers.

The application owns persistence of the selected locale, validation of saved values, any system-language mapping, and the list of live views/canvases. Set the desired language before importing newly opened views. A custom locale-change event can coordinate the refresh, but it is not provided by SampleFW `CLocalize` itself.

If callbacks capture a view through `[this]`, clear `m_canvas->OnLocalize` before that view dies when the canvas can outlive it. Stop refresh callbacks, destroy/detach GUI owners, and only then release custom font caches and localization data. Release the `CLocalize` singleton through `CLocalize::releaseInstance()` in the owning application's shutdown sequence, after its consumers stop.

### Refresh all canvases in a scene

The engine API to call again is **`CCanvas::updateLocalizedText()`**, declared in [CCanvas.h](../Projects/Skylicht/Engine/Graphics2D/CCanvas.h). The `Documents/Media/Doxygen/Graphics2D/CCanvas` directory contains documentation images; the method itself is implemented in the engine's `CCanvas.cpp`.

A settings/language picker can collect canvases from the scene hierarchy instead of holding a separate list. The following method belongs to an application-defined `CLanguageSettings` controller:

```cpp
void CLanguageSettings::refreshAllCanvas(CScene* scene)
{
	if (!scene)
		return;

	for (int i = 0; i < scene->getZoneCount(); i++)
	{
		CZone* zone = scene->getZone(i);
		std::vector<CCanvas*> canvases = zone->getComponentsInChild<CCanvas>(true);
		for (CCanvas* canvas : canvases)
			canvas->updateLocalizedText();
	}
}
```

Include the scene/zone definitions and `Graphics2D/CCanvas.h`. `getComponentsInChild<CCanvas>(false)` collects descendant canvases while excluding a component on the zone itself; `true` also includes the zone's own component. The example handles all zones rather than assuming every GUI belongs to zone 0. Refresh other live scenes separately if they own UI that must change too. Hierarchy collection does not filter by visible state, so hidden menus under these zones are refreshed as well.

Call this after assigning the selected language:

```cpp
CLocalize::getInstance()->setLanguage(selectedLanguage);
refreshAllCanvas(scene);
```

Use the same sequence when restoring the old language on Cancel/Back. Save the previous enum when opening the picker, preview each selection immediately, persist the selected value on confirmation, and restore both application setting state and `CLocalize` on cancellation. Persistence and confirmation are application behavior; neither `CCanvas` nor `CLocalize` provides them automatically.

Run the refresh on the normal UI/main thread, while the scene and callback owners remain valid. Keep callbacks focused on text/font/layout updates; do not destroy canvases or rebuild the traversed hierarchy during this refresh.

### Refresh dynamic text through a view-specific callback

A settings view can override the common localization handler to rebuild text composed from runtime state. The common handler first applies the locale font and standard translation; the view-specific part then replaces a dynamic status label with its freshly composed value:

```cpp
void CSettingsView::onLocalize(CGUIText* text)
{
	CBaseView::onLocalize(text);

	if (text == m_statusText)
		updateStatusText();
}
```

`CSettingsView`, `CBaseView`, `m_statusText`, and `updateStatusText()` are illustrative application members. Bind the canvas callback to this view-specific handler. Its status updater should derive the message from current application state and translated keys each time, rather than translating the previously displayed string. For colored formatted text, set its color-format entries with `setColorFormat(...)` and reapply the message with `setFormatText(...)` after the common handler, since ordinary `setText()` resets formatting.

Initialize `m_statusText` to `NULL`, and assign it after GUI import before explicitly refreshing existing text. The import callback can run before that pointer is assigned or before other view controls exist, so its view-specific code must tolerate partial initialization. If replacing the callback after import, call `updateLocalizedText()` once to apply the new handler immediately. Do not call `updateLocalizedText()` recursively from inside `OnLocalize`.

## 7. Verify localization

- Open a GUI after selecting each language and verify the initial import callback.
- Change languages repeatedly with menus and popups already open.
- Test missing IDs, empty cells, CSV load failure, and missing font assets.
- Verify accents, script-specific glyphs, punctuation, wrapping, and long labels.
- Switch from a replacement font back to the original font and verify style/size.
- Check unkeyed dynamic labels, trimmed strings, and layout after resizing.
- Reopen views and confirm callbacks and cached fonts have valid lifetimes.

## Public source references

| Source | What it demonstrates |
| --- | --- |
| [SampleFW CLocalize.h](../Samples/SampleFW/Localize/CLocalize.h), [CLocalize.cpp](../Samples/SampleFW/Localize/CLocalize.cpp) | Public language enum, lookup overloads, and data mapping |
| [CBaseConfig.cpp](../Samples/SampleFW/Config/CBaseConfig.cpp) | CSV loader and owned records |
| [GUI sample CViewBase.cpp](../Samples/GUI/Source/CViewBase.cpp) | Bind localization before GUI import |
| [SampleGUI.cpp](../Samples/GUI/Source/SampleGUI.cpp) | Localization singleton lifecycle |
| [CCanvas.cpp](../Projects/Skylicht/Engine/Graphics2D/CCanvas.cpp) | Existing-label refresh traversal |
| [CGUIText.h](../Projects/Skylicht/Engine/Graphics2D/GUI/CGUIText.h), [CGUIText.cpp](../Projects/Skylicht/Engine/Graphics2D/GUI/CGUIText.cpp) | Text IDs, UTF-8 setters, trimming, import callback, and custom font |
| [CFontSource.cpp](../Projects/Skylicht/Engine/Graphics2D/SpriteFrame/CFontSource.cpp) | Font initialization and lifetime |
