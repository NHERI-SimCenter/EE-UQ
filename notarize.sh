# remove & rebuild app and macdeploy it

APP_NAME="EE_UQ"
APP_FILE="EE_UQ.app"
DMG_FILENAME="${APP_NAME}_Mac_Download.dmg"

userID="userID.sh"

source $userID

echo "xcrun notarytool submit ./$DMG_FILENAME --apple-id $appleID --team-id $appleCredential  --password $appleAppPassword"
echo "xcrun notarytool log ID --apple-id $appleID --team-id $appleCredential  --password $appleAppPassword"
echo ""
echo "NO MORE EMAILS .. have to check the log file"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple \"$APP_NAME\" $DMG_FILENAME"
